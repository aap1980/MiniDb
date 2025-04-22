#include "SelectStatement.h"
#include "../Database/Database.h"
#include "../SqlParser/SQLParser.h"
#include "../Table/QueryResult.h"
#include "../Table/Columns.h"
#include "QueryTables.h"
#include "SelectedColumns.h"
#include "JoinCondition.h"
#include "WhereCondition.h"
#include <iostream>
#include <optional>
#include <functional>
#include <map>
#include <variant>

namespace MiniDb::Statement {

	SelectStatement::SelectStatement(std::unique_ptr<hsql::SQLParserResult> parserResult) {
		_parserResult = std::move(parserResult);
		_statement = static_cast<const hsql::SelectStatement*>(_parserResult->getStatement(0));
	}

	// Kontekst dla aktualnie przetwarzanego połączonego wiersza (mapa alias -> wskaźnik na Row)
	using ActiveJoinRowsMap = std::map<std::string, const MiniDb::Table::Row*>;

	// Pobiera wartość z odpowiedniego wiersza w kontekście
	const std::string& getValueFromContext(
		const ActiveJoinRowsMap& activeRowsMap,
		const std::string& tableAlias,
		const std::string& columnName,
		const MiniDb::Table::Columns& columns)
	{
		// Aktualny wiersz dla danego aliasu w kontekście JOIN
		auto rowIt = activeRowsMap.find(tableAlias);
		if (rowIt == activeRowsMap.end()) {
			throw std::runtime_error("Internal error: Cannot find row for table alias '" + tableAlias + "' in the current join context.");
		}
		const MiniDb::Table::Row* row = rowIt->second;

		size_t colIndex = columns.getColumnIndexByName(columnName);
		return row->getValueByIndex(colIndex);
	}

	// Ewaluuje warunek JOIN
	bool evaluateJoinCondition(
		MiniDb::Statement::QueryTables& queryTables,
		const MiniDb::Statement::JoinCondition& joinCondition,
		const ActiveJoinRowsMap& activeRowsMap)
	{
		if (joinCondition.operatorType != hsql::kOpEquals) {
			throw std::runtime_error("Only '=' operator is supported in JOIN ON conditions.");
		}

		MiniDb::Statement::QueryTable& leftTable = queryTables.getByAlias(joinCondition.leftTableAlias);
		const std::string& leftValue = getValueFromContext(activeRowsMap, joinCondition.leftTableAlias, joinCondition.leftColumnName, leftTable.table.columns);
		MiniDb::Statement::QueryTable& righTable = queryTables.getByAlias(joinCondition.rightTableAlias);
		const std::string& rightValue = getValueFromContext(activeRowsMap, joinCondition.rightTableAlias, joinCondition.rightColumnName, righTable.table.columns);

		// Proste porównanie stringów - W REALNEJ BAZIE WYMAGA OBSŁUGI TYPÓW!
		return leftValue == rightValue;
	}

	// Ewaluuje warunek WHERE
	bool evaluateWhereCondition(
		const MiniDb::Statement::WhereCondition& cond,
		const ActiveJoinRowsMap& activeRowsMap,
		const MiniDb::Table::Columns& columns)
	{
		const std::string& columnValueStr = getValueFromContext(activeRowsMap, cond.tableAlias, cond.columnName, columns);

		// BARDZO UPROSZCZONA EWALUACJA - WYMAGA OBSŁUGI TYPÓW I BŁĘDÓW KONWERSJI!
		try {
			if (std::holds_alternative<long>(cond.literalValue)) {
				long literalValue = std::get<long>(cond.literalValue);
				long columnValue = std::stol(columnValueStr);
				switch (cond.operatorType) {
				case hsql::kOpEquals: return columnValue == literalValue;
				case hsql::kOpNotEquals: return columnValue != literalValue;
				case hsql::kOpLess: return columnValue < literalValue;
				case hsql::kOpLessEq: return columnValue <= literalValue;
				case hsql::kOpGreater: return columnValue > literalValue;
				case hsql::kOpGreaterEq: return columnValue >= literalValue;
				default: throw std::runtime_error("Unsupported WHERE operator for INT.");
				}
			}
			else if (std::holds_alternative<std::string>(cond.literalValue)) {
				const std::string& literalValue = std::get<std::string>(cond.literalValue);
				switch (cond.operatorType) {
				case hsql::kOpEquals: return columnValueStr == literalValue;
				case hsql::kOpNotEquals: return columnValueStr != literalValue;
				default: throw std::runtime_error("Unsupported WHERE operator for STRING.");
				}
			}
			// TODO: dodać obsługę double jeśli jest potrzebna
			else {
				throw std::runtime_error("Unsupported literal type in WHERE clause.");
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Warning: Type conversion or comparison error in WHERE for value '" << columnValueStr << "': " << e.what() << std::endl;
			return false;
		}
	}

	std::unique_ptr<MiniDb::Table::QueryResult> SelectStatement::execute(MiniDb::Database::Database& database) const {
		if (!_statement || !_statement->fromTable) {
			throw std::runtime_error("Invalid SELECT statement structure: missing FROM clause.");
		}

		MiniDb::Statement::QueryTables queryTables;

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

				MiniDb::Table::Table& table = database.getTable(tableName);
				table.loadDataFromFile();

				// Dodajemy tabelę bez warunku JOIN, bo to albo tabela bazowa, albo prawa strona JOINa,
				// której warunek zostanie dodany później w sekcji kTableJoin
				QueryTable queryTableInfo(tableAlias, tableName, table);
				queryTables.addTable(queryTableInfo);
				break;
			}

			case hsql::kTableJoin: {
				// Przypadek rekurencyjny: Znaleziono JOIN
				const hsql::JoinDefinition* joinDefinition = ref->join;

				// 1. Przetwórz najpierw lewą stronę JOINa (rekurencyjnie)
				// To zapewni, że tabela bazowa i kolejne złączenia są dodawane w poprawnej kolejności
				processTableRefRecursively(joinDefinition->left);

				// 2. Sprawdź poprawność prawej strony (oczekujemy prostej tabeli)
				if (joinDefinition->right == nullptr || joinDefinition->right->type != hsql::kTableName) {
					throw std::runtime_error("Invalid right side of JOIN clause (expected simple table reference).");
				}
				const hsql::TableRef* rightTableRef = joinDefinition->right;

				// 3. Przetwórz prawą stronę JOINa (dodaj tabelę do struktur)
				// Wywołanie rekurencyjne obsłuży załadowanie i dodanie tabeli
				processTableRefRecursively(rightTableRef);

				// 4. Po przetworzeniu prawej strony, tabela powinna być ostatnią w queryTablesOrder.
				// Teraz dodajmy do niej warunek JOIN.
				if (queryTables.tables.empty()) {
					throw std::logic_error("Internal error: queryTablesOrder is empty after processing JOIN's right side.");
				}

				// Sprawdź typ JOIN (na razie tylko INNER)
				if (joinDefinition->type != hsql::kJoinInner) {
					throw std::runtime_error("Only INNER JOIN is supported.");
				}

				// 5. Parsuj warunek ON dla tego konkretnego JOINa
				if (!joinDefinition->condition || joinDefinition->condition->type != hsql::kExprOperator || joinDefinition->condition->opType != hsql::kOpEquals ||
					!joinDefinition->condition->expr || joinDefinition->condition->expr->type != hsql::kExprColumnRef ||
					!joinDefinition->condition->expr2 || joinDefinition->condition->expr2->type != hsql::kExprColumnRef) {
					throw std::runtime_error("JOIN condition must be a simple column equality (alias1.col1 = alias2.col2).");
				}

				const hsql::Expr* leftColExpr = joinDefinition->condition->expr;
				const hsql::Expr* rightColExpr = joinDefinition->condition->expr2;
				std::string leftTableAlias = leftColExpr->table ? leftColExpr->table : "";
				std::string leftColumnName = leftColExpr->name;
				std::string rightTableAlias = rightColExpr->table ? rightColExpr->table : "";
				std::string rightColumnName = rightColExpr->name;

				if (leftTableAlias.empty() || rightTableAlias.empty() || leftColumnName.empty() || rightColumnName.empty()) {
					throw std::runtime_error("Columns in JOIN ON condition must be qualified with table aliases.");
				}

				// Sprawdź, czy aliasy w warunku ON istnieją
				if (!queryTables.hasAlias(leftTableAlias)) {
					throw std::runtime_error("Unknown table alias '" + leftTableAlias + "' in JOIN ON condition.");
				}
				if (!queryTables.hasAlias(rightTableAlias)) {
					throw std::runtime_error("Unknown table alias '" + rightTableAlias + "' in JOIN ON condition.");
				}

				// Upewnij się, że jedna strona warunku odnosi się do tabeli dodanej w tym kroku
				bool conditionLinksNewTable = (leftTableAlias == queryTables.last().tableAlias || rightTableAlias == queryTables.last().tableAlias);
				if (!conditionLinksNewTable) {
					throw std::runtime_error("JOIN ON condition for table '" + queryTables.last().tableAlias + "' does not reference it.");
				}

				// Przypisz sparsowany warunek do informacji o ostatnio dodanej tabeli
				JoinCondition joinCondition(leftTableAlias, leftColumnName, rightTableAlias, rightColumnName, joinDefinition->condition->opType);
				queryTables.last().joinCondition = std::move(joinCondition);

				break;
			}

								 // Obsługa innych, nieimplementowanych typów TableRef
			case hsql::kTableCrossProduct:
				throw std::runtime_error("CROSS JOIN (comma-separated tables in FROM) is not supported.");
			default:
				throw std::runtime_error("Unsupported FROM clause structure type.");
			}
			};

		processTableRefRecursively(_statement->fromTable);

		// 3. Przetwórz listę SELECT
		MiniDb::Table::Columns resultColumnsDefinition; // Definicje kolumn dla wyniku
		MiniDb::Statement::SelectedColumns selectedColumns; // Co dokładnie wybrać i skąd

		if (!_statement->selectList) {
			throw std::runtime_error("Missing SELECT list.");
		}

		bool selectAll = _statement->selectList->size() == 1 && (*_statement->selectList)[0]->type == hsql::kExprStar;

		if (selectAll) {
			for (const auto& queryTableInfo : queryTables.tables) {
				for (const auto& column : queryTableInfo.table.columns.getColumns()) {
					// Tworzymy nową definicję dla wyniku, potencjalnie z prefiksem aliasu dla jasności
					MiniDb::Table::Column resultColDef = column; // Kopiujemy metadane
					resultColDef.name = queryTableInfo.tableAlias + "." + column.name; // Np. "u.login"
					resultColumnsDefinition.addColumn(resultColDef);
					MiniDb::Statement::SelectedColumn selectedColumnInfo(queryTableInfo.tableAlias, column.name, column);
					selectedColumns.addColumn(selectedColumnInfo);
				}
			}
		}
		else {
			for (const hsql::Expr* expr : *_statement->selectList) {
				if (expr->type == hsql::kExprColumnRef) {
					std::string columnName = expr->name;
					std::string tableAlias = expr->table ? expr->table : "";

					if (tableAlias.empty()) {
						// Można by próbować rozwiązać niejednoznaczność, jeśli nazwa jest unikalna we wszystkich tabelach,
						// ale wymaganie aliasu przy JOIN jest bezpieczniejsze.
						throw std::runtime_error("Column '" + columnName + "' in SELECT list must be qualified with a table alias when using JOIN.");
					}

					const MiniDb::Table::Table& sourceTable = queryTables.getByAlias(tableAlias).table;
					const MiniDb::Table::Column& sourceColumn = sourceTable.columns.getColumnByName(columnName);
					resultColumnsDefinition.addColumn(sourceColumn);
					MiniDb::Statement::SelectedColumn selectedColumnInfo(tableAlias, columnName, sourceColumn);
					selectedColumns.addColumn(selectedColumnInfo);
				}
				else {
					throw std::runtime_error("Unsupported expression type in SELECT list. Only columns (alias.name) or * are supported.");
				}
			}
		}

		// 4. Przetwórz klauzulę WHERE
		std::optional<MiniDb::Statement::WhereCondition> whereConditionOptional;

		if (_statement->whereClause != nullptr) {
			const auto* clause = _statement->whereClause;
			// Uproszczone parsowanie: zakładamy column op literal
			if (clause->type == hsql::kExprOperator &&
				clause->opType != hsql::kOpOr && clause->opType != hsql::kOpAnd &&
				clause->expr && clause->expr->type == hsql::kExprColumnRef &&
				clause->expr2 && (clause->expr2->type == hsql::kExprLiteralInt || clause->expr2->type == hsql::kExprLiteralString || clause->expr2->type == hsql::kExprLiteralFloat))
			{
				std::string colName = clause->expr->name;
				std::string tableAlias = clause->expr->table ? clause->expr->table : "";

				if (tableAlias.empty()) {
					throw std::runtime_error("Column '" + colName + "' in WHERE clause must be qualified with a table alias when using JOIN.");
				}
				if (!queryTables.hasAlias(tableAlias)) {
					throw std::runtime_error("Unknown table alias '" + tableAlias + "' in JOIN ON condition.");
				}

				LiteralValue literal;

				switch (clause->expr2->type) {
				case hsql::kExprLiteralInt:
					literal = static_cast<long>(clause->expr2->ival);
					break;
				case hsql::kExprLiteralFloat:
					literal = clause->expr2->fval;
					break;
				case hsql::kExprLiteralString:
					literal = std::string(clause->expr2->name);
					break;
				default:
					throw std::runtime_error("Unsupported literal type in WHERE clause.");
				}

				whereConditionOptional.emplace(MiniDb::Statement::WhereCondition(tableAlias, colName, clause->opType, literal));

			}
			else {
				throw std::runtime_error("Unsupported WHERE clause structure. Expected: alias.column OPERATOR literal.");
			}
		}

		// 5. Wykonaj złączenie (Nested Loop) i filtrowanie
		auto queryResult = std::make_unique<MiniDb::Table::QueryResult>();
		queryResult->columns = resultColumnsDefinition; // Ustaw metadane kolumn wyniku

		// Rekurencyjna funkcja pomocnicza do iteracji
		std::function<void(size_t, ActiveJoinRowsMap)> processJoinLevel =
			[&](size_t currentTableIndex, ActiveJoinRowsMap activeRowsMap)
			{
				const QueryTable& currentTableInfo = queryTables.getByIndex(currentTableIndex);

				for (const MiniDb::Table::Row& row : currentTableInfo.table.rows.getRows()) {
					activeRowsMap[currentTableInfo.tableAlias] = &row;

					bool joinOk = true;
					if (currentTableIndex > 0) { // Sprawdź warunek JOIN dla tabel > 0
						// joinCondition powinno być ustawione w QueryTableInfo dla tabel > 0
						if (!currentTableInfo.joinCondition.has_value()) {
							throw std::logic_error("Internal error: Missing JOIN condition for table " + currentTableInfo.tableAlias);
						}
						joinOk = evaluateJoinCondition(queryTables, currentTableInfo.joinCondition.value(), activeRowsMap);
					}

					if (joinOk) {
						if (currentTableIndex + 1 < queryTables.size()) {
							// Przejdź do następnej tabeli w JOIN
							processJoinLevel(currentTableIndex + 1, activeRowsMap);
						}
						else {
							// Ostatnia tabela - mamy pełny połączony wiersz (w currentContext)
							// Zastosuj WHERE
							bool whereOk = true;
							if (whereConditionOptional) {
								const MiniDb::Table::Table& whereTable = queryTables.getByAlias(whereConditionOptional.value().tableAlias).table;
								whereOk = evaluateWhereCondition(whereConditionOptional.value(), activeRowsMap, whereTable.columns);
							}

							if (whereOk) {
								// Zbuduj wiersz wynikowy
								std::vector<std::string> resultRowValues;
								resultRowValues.reserve(selectedColumns.size());
								for (const auto& column : selectedColumns.columns) {
									const MiniDb::Table::Table& table = queryTables.getByAlias(column.tableAlias).table;
									resultRowValues.push_back(
										getValueFromContext(activeRowsMap, column.tableAlias, column.columnName, table.columns)
									);
								}
								// Dodaj wiersz do wyniku używając konstruktora Row z MiniDb
								queryResult->rows.addRow(MiniDb::Table::Row(std::move(resultRowValues)));
							}
						}
					}
				}
				// Po zakończeniu pętli dla danego poziomu, kontekst dla tego aliasu
				// zostanie automatycznie nadpisany w następnej iteracji pętli zewnętrznej
				// lub funkcja zakończy działanie. Nie trzeba jawnie usuwać.
			};

		// Rozpocznij rekurencję od pierwszej tabeli (indeks 0), jeśli istnieją tabele
		if (!queryTables.tables.empty()) {
			ActiveJoinRowsMap initialRowsMap;
			processJoinLevel(0, initialRowsMap);
		}

		// 6. Zwróć wynik
		return queryResult;



		// 1. Pobierz tabelę
		/*std::string tableAlias;
		if (_statement->fromTable->alias != nullptr) {
			tableAlias = _statement->fromTable->alias->name;
		}

		std::string tableName;
		tableName = _statement->fromTable->name;

		MiniDb::Table::Table table = database.getTable(tableName);
		table.loadDataFromFile();
		const auto& columns = table.columns.getColumns();*/

		// 2. Określ kolumny do SELECT
		/*MiniDb::Table::Columns selectedColumns;

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
		}*/

		// 3. WHERE — przygotuj filtr (jeśli jest)
		// expr – lewa strona to kolumna
		// expr2 – prawa strona to wartość
		/*std::optional<std::function<bool(const Table::Row&)>> rowFilter;

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
		}*/

		// 4. Składanie QueryResult
		/*MiniDb::Table::QueryResult queryResult;
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
		}*/

		//return std::make_unique<MiniDb::Table::QueryResult>(std::move(queryResult));
	}

}
