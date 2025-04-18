#include "SelectStatement.h"
#include "../Database/Database.h"
#include "../SqlParser/SQLParser.h"
#include "../Table/QueryResult.h"
#include "../Table/Columns.h"
#include <iostream>
#include <optional>
#include <functional>
#include <map>

namespace MiniDb::Statement {

	SelectStatement::SelectStatement(std::unique_ptr<hsql::SQLParserResult> parserResult) {
		_parserResult = std::move(parserResult);
		_statement = static_cast<const hsql::SelectStatement*>(_parserResult->getStatement(0));
	}

	// Reprezentuje sparsowany warunek JOIN ON
	struct ParsedJoinCondition {
		std::string leftTableAlias;
		std::string leftColumnName;
		std::string rightTableAlias;
		std::string rightColumnName;
		hsql::OperatorType opType = hsql::kOpEquals;
	};

	// Informacje o tabeli biorącej udział w zapytaniu
	struct QueryTableInfo {
		std::string alias;
		std::string tableName;
		MiniDb::Table::Table table;
		std::optional<ParsedJoinCondition> joinCondition; // Warunek łączący z *poprzednią* tabelą w kolejności
	};

	std::unique_ptr<MiniDb::Table::QueryResult> SelectStatement::execute(MiniDb::Database::Database& database) const {
		if (!_statement || !_statement->fromTable) {
			throw std::runtime_error("Invalid SELECT statement structure: missing FROM clause.");
		}

		// 1. Przygotuj struktury do przechowywania informacji o zapytaniu
		std::vector<QueryTableInfo> queryTablesOrder; // Kolejność tabel jak w zapytaniu (FROM, JOIN1, JOIN2...)
		std::map<std::string, size_t> aliasToIndex; // Mapowanie aliasu na indeks w queryTablesOrder
		std::map<std::string, const MiniDb::Table::Columns*> tableColumnsMap; // Cache definicji kolumn per alias

		// Rekurencyjne przetwarzanie FROM/JOIN
		std::function<void(const hsql::TableRef*)> processTableRefRecursively;

		processTableRefRecursively =
			[&](const hsql::TableRef* ref) {
			if (ref == nullptr) {
				throw std::runtime_error("Internal error: Encountered null TableRef during FROM clause processing.");
			}

			switch (ref->type) {
			case hsql::kTableName: {
				// Podstawowy przypadek: Znaleziono referencję do tabeli
				const hsql::TableRef* tableRef = ref;
				std::string tableName = tableRef->name;
				std::string tableAlias = (tableRef->alias != nullptr) ? tableRef->alias->name : tableName;

				if (aliasToIndex.count(tableAlias)) {
					// Ten alias już istnieje (może się zdarzyć, jeśli jest to prawa strona JOINa,
					// która została już przetworzona - chociaż logika powinna temu zapobiegać).
					// Lub jest to faktyczny duplikat w zapytaniu.
					throw std::runtime_error("Duplicate table alias detected: " + tableAlias);
				}

				MiniDb::Table::Table table = database.getTable(tableName);
				table.loadDataFromFile();

				size_t currentIndex = queryTablesOrder.size();
				// Dodajemy tabelę bez warunku JOIN (bo to albo tabela bazowa, albo prawa strona JOINa,
				// której warunek zostanie dodany później w sekcji kTableJoin)
				queryTablesOrder.push_back({ tableAlias, tableName, std::move(table), std::nullopt });
				aliasToIndex[tableAlias] = currentIndex;
				tableColumnsMap[tableAlias] = &queryTablesOrder[currentIndex].table.columns;
				break;
			}

			case hsql::kTableJoin: {
				// Przypadek rekurencyjny: Znaleziono JOIN
				const hsql::JoinDefinition* joinDef = ref->join;

				// 1. Przetwórz najpierw lewą stronę JOINa (rekurencyjnie)
				// To zapewni, że tabela bazowa i kolejne złączenia są dodawane w poprawnej kolejności
				processTableRefRecursively(joinDef->left);

				// 2. Sprawdź poprawność prawej strony (oczekujemy prostej tabeli)
				if (joinDef->right == nullptr || joinDef->right->type != hsql::kTableName) {
					throw std::runtime_error("Invalid right side of JOIN clause (expected simple table reference).");
				}
				const hsql::TableRef* rightTableRef = joinDef->right;

				// 3. Przetwórz prawą stronę JOINa (dodaj tabelę do struktur)
				// Wywołanie rekurencyjne obsłuży załadowanie i dodanie tabeli
				processTableRefRecursively(rightTableRef);

				// 4. Po przetworzeniu prawej strony, tabela powinna być ostatnią w queryTablesOrder.
				// Teraz dodajmy do niej warunek JOIN.
				if (queryTablesOrder.empty()) {
					throw std::logic_error("Internal error: queryTablesOrder is empty after processing JOIN's right side.");
				}
				QueryTableInfo& newlyAddedTableInfo = queryTablesOrder.back(); // Ostatnio dodana tabela (z prawej strony JOIN)

				// Sprawdź typ JOIN (na razie tylko INNER)
				if (joinDef->type != hsql::kJoinInner) {
					throw std::runtime_error("Only INNER JOIN is supported.");
				}

				// 5. Parsuj warunek ON dla tego konkretnego JOINa
				if (!joinDef->condition || joinDef->condition->type != hsql::kExprOperator || joinDef->condition->opType != hsql::kOpEquals ||
					!joinDef->condition->expr || joinDef->condition->expr->type != hsql::kExprColumnRef ||
					!joinDef->condition->expr2 || joinDef->condition->expr2->type != hsql::kExprColumnRef) {
					throw std::runtime_error("JOIN condition must be a simple column equality (alias1.col1 = alias2.col2).");
				}

				const hsql::Expr* leftColExpr = joinDef->condition->expr;
				const hsql::Expr* rightColExpr = joinDef->condition->expr2;
				std::string leftAlias = leftColExpr->table ? leftColExpr->table : "";
				std::string leftCol = leftColExpr->name;
				std::string rightAlias = rightColExpr->table ? rightColExpr->table : "";
				std::string rightCol = rightColExpr->name;

				if (leftAlias.empty() || rightAlias.empty() || leftCol.empty() || rightCol.empty()) {
					throw std::runtime_error("Columns in JOIN ON condition must be qualified with table aliases.");
				}

				// Sprawdź, czy aliasy w warunku ON istnieją w mapie `aliasToIndex`
				// (powinny istnieć, bo obie strony JOIN zostały już przetworzone)
				if (!aliasToIndex.count(leftAlias)) {
					throw std::runtime_error("Unknown table alias '" + leftAlias + "' in JOIN ON condition.");
				}
				if (!aliasToIndex.count(rightAlias)) {
					throw std::runtime_error("Unknown table alias '" + rightAlias + "' in JOIN ON condition.");
				}
				// Upewnij się, że jedna strona warunku odnosi się do tabeli dodanej w tym kroku (newlyAddedTableInfo)
				bool conditionLinksNewTable = (leftAlias == newlyAddedTableInfo.alias || rightAlias == newlyAddedTableInfo.alias);
				if (!conditionLinksNewTable) {
					throw std::runtime_error("JOIN ON condition for table '" + newlyAddedTableInfo.alias + "' does not reference it.");
				}


				ParsedJoinCondition condition = { leftAlias, leftCol, rightAlias, rightCol, joinDef->condition->opType };

				// Przypisz sparsowany warunek do informacji o ostatnio dodanej tabeli
				newlyAddedTableInfo.joinCondition = std::move(condition);

				break; // Koniec obsługi kTableJoin
			}

								 // Obsługa innych, nieimplementowanych typów TableRef
			case hsql::kTableCrossProduct:
				throw std::runtime_error("CROSS JOIN (comma-separated tables in FROM) is not supported.");
			default:
				throw std::runtime_error("Unsupported FROM clause structure type.");
			}
			};

		processTableRefRecursively(_statement->fromTable);


		// 1. Pobierz tabelę
		std::string tableAlias;
		if (_statement->fromTable->alias != nullptr) {
			tableAlias = _statement->fromTable->alias->name;
		}

		std::string tableName;
		tableName = _statement->fromTable->name;

		MiniDb::Table::Table table = database.getTable(tableName);
		table.loadDataFromFile();
		const auto& columns = table.columns.getColumns();

		// 2. Określ kolumny do SELECT
		MiniDb::Table::Columns selectedColumns;

		bool selectAll = _statement->selectList->size() == 1 && (*_statement->selectList)[0]->type == hsql::kExprStar;

		if (selectAll) {
			selectedColumns.addColumns(table.columns.getColumns());
		}
		else {
			for (const auto* expr : *_statement->selectList) {
				if (expr->type == hsql::kExprColumnRef) {
					std::string columnName = expr->name;
					std::string columnPrefix = expr->table;
					if (!columnPrefix.empty() && columnPrefix != tableAlias) {
						throw std::runtime_error("Unknown table alias: " + columnPrefix);
					}
					selectedColumns.addColumn(table.columns.getColumnByName(columnName));
				}
				else {
					throw std::runtime_error("Unsupported SELECT expression. Only column references or '*' are supported.");
				}
			}
		}

		// 3. WHERE — przygotuj filtr (jeśli jest)
		// expr – lewa strona to kolumna
		// expr2 – prawa strona to wartość
		std::optional<std::function<bool(const Table::Row&)>> rowFilter;

		if (_statement->whereClause != nullptr) {
			const auto* whereClause = _statement->whereClause;

			if (whereClause->type == hsql::kExprOperator &&
				whereClause->opType != hsql::kOpOr && whereClause->opType != hsql::kOpAnd &&
				whereClause->expr && whereClause->expr2 &&
				whereClause->expr->type == hsql::kExprColumnRef &&
				whereClause->expr2->type == hsql::kExprLiteralInt) {

				std::string columnName = whereClause->expr->name;
				std::string columnPrefix = whereClause->expr->table;
				int literal = whereClause->expr2->ival;

				if (!columnPrefix.empty() && columnPrefix != tableAlias) {
					throw std::runtime_error("Unknown table alias: " + columnPrefix);
				}

				auto columnIndex = table.columns.getColumnIndexByName(columnName);

				// Tworzymy lambda funkcję filtrującą
				switch (whereClause->opType) {
				case hsql::kOpEquals:
					rowFilter = [columnIndex, literal](const Table::Row& row) {
						return std::stoi(row.getValueByIndex(columnIndex)) == literal;
						};
					break;
				case hsql::kOpGreater:
					rowFilter = [columnIndex, literal](const Table::Row& row) {
						return std::stoi(row.getValueByIndex(columnIndex)) > literal;
						};
					break;
				case hsql::kOpLess:
					rowFilter = [columnIndex, literal](const Table::Row& row) {
						return std::stoi(row.getValueByIndex(columnIndex)) < literal;
						};
					break;
				default:
					throw std::runtime_error("Unsupported WHERE operator.");
				}

			}
			else {
				throw std::runtime_error("Unsupported WHERE clause format.");
			}
		}

		// 4. Składanie QueryResult
		MiniDb::Table::QueryResult queryResult;
		queryResult.columns = selectedColumns;

		for (const auto& row : table.rows.getRows()) {
			if (rowFilter && !(*rowFilter)(row)) {
				continue;
			}

			std::vector<std::string> resultRow;
			for (const auto& column : selectedColumns.getColumns()) {
				auto columnIndex = table.columns.getColumnIndexByName(column.name);
				resultRow.push_back(row.getValueByIndex(columnIndex));
			}

			queryResult.rows.addRow(Table::Row(std::move(resultRow)));
		}

		return std::make_unique<MiniDb::Table::QueryResult>(std::move(queryResult));
	}

}
