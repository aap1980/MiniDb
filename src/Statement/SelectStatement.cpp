#include "SelectStatement.h"
#include "../Database/Database.h"
#include "../SqlParser/SQLParser.h"
#include "../Table/QueryResult.h"
#include "../Table/Columns.h"
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

	// Reprezentuje sparsowany warunek JOIN ON
	struct ParsedJoinCondition {
		std::string leftTableAlias;
		std::string leftColumnName;
		std::string rightTableAlias;
		std::string rightColumnName;
		hsql::OperatorType opType = hsql::kOpEquals;
	};

	// Reprezentuje sparsowany warunek WHERE
	using LiteralValue = std::variant<long, double, std::string>; // Użyj std::variant lub podobnego
	struct ParsedWhereCondition {
		std::string tableAlias;
		std::string columnName;
		hsql::OperatorType opType;
		LiteralValue value;
	};

	// Informacje o kolumnie wybranej w SELECT
	struct SelectedColumnInfo {
		std::string sourceTableAlias;
		std::string sourceColumnName;
		MiniDb::Table::Column originalDefinition; // Przechowujemy oryginalną definicję
	};

	// Informacje o tabeli biorącej udział w zapytaniu
	struct QueryTableInfo {
		std::string alias;
		std::string tableName;
		MiniDb::Table::Table table;
		std::optional<ParsedJoinCondition> joinCondition; // Warunek łączący z *poprzednią* tabelą w kolejności
	};

	// Kontekst dla aktualnie przetwarzanego połączonego wiersza (mapa alias -> wskaźnik na Row)
	using JoinedRowContext = std::map<std::string, const MiniDb::Table::Row*>;

	// Pobiera wartość z odpowiedniego wiersza w kontekście
	const std::string& getValueFromContext(
		const JoinedRowContext& context,
		const std::string& tableAlias,
		const std::string& columnName,
		const std::map<std::string, const MiniDb::Table::Columns*>& columnInfos)
	{
		auto rowIt = context.find(tableAlias);
		auto colInfoIt = columnInfos.find(tableAlias);
		if (rowIt == context.end() || colInfoIt == columnInfos.end()) {
			throw std::runtime_error("Internal error: Cannot find table '" + tableAlias + "' in join context.");
		}
		const MiniDb::Table::Row* row = rowIt->second;
		const MiniDb::Table::Columns* columns = colInfoIt->second;
		try {
			size_t colIndex = columns->getColumnIndexByName(columnName);
			return row->getValueByIndex(colIndex);
		}
		catch (const std::runtime_error& e) {
			throw std::runtime_error("Internal error: Cannot find column '" + columnName + "' in table '" + tableAlias + "' during context lookup.");
		}
	}

	// Ewaluuje warunek JOIN
	bool evaluateJoinCondition(
		const ParsedJoinCondition& cond,
		const JoinedRowContext& context,
		const std::map<std::string, const MiniDb::Table::Columns*>& columnInfos)
	{
		// TODO: Dodać obsługę różnych typów operatorów jeśli potrzeba
		if (cond.opType != hsql::kOpEquals) {
			throw std::runtime_error("Only '=' operator is supported in JOIN ON conditions.");
		}

		const std::string& leftValue = getValueFromContext(context, cond.leftTableAlias, cond.leftColumnName, columnInfos);
		const std::string& rightValue = getValueFromContext(context, cond.rightTableAlias, cond.rightColumnName, columnInfos);

		// Proste porównanie stringów - W REALNEJ BAZIE WYMAGA OBSŁUGI TYPÓW!
		return leftValue == rightValue;
	}

	// Ewaluuje warunek WHERE
	bool evaluateWhereCondition(
		const ParsedWhereCondition& cond,
		const JoinedRowContext& context,
		const std::map<std::string, const MiniDb::Table::Columns*>& columnInfos)
	{
		const std::string& columnValueStr = getValueFromContext(context, cond.tableAlias, cond.columnName, columnInfos);

		// BARDZO UPROSZCZONA EWALUACJA - WYMAGA OBSŁUGI TYPÓW I BŁĘDÓW KONWERSJI!
		try {
			if (std::holds_alternative<long>(cond.value)) {
				long literalValue = std::get<long>(cond.value);
				long columnValue = std::stol(columnValueStr);
				switch (cond.opType) {
				case hsql::kOpEquals: return columnValue == literalValue;
				case hsql::kOpNotEquals: return columnValue != literalValue;
				case hsql::kOpLess: return columnValue < literalValue;
				case hsql::kOpLessEq: return columnValue <= literalValue;
				case hsql::kOpGreater: return columnValue > literalValue;
				case hsql::kOpGreaterEq: return columnValue >= literalValue;
				default: throw std::runtime_error("Unsupported WHERE operator for INT.");
				}
			}
			else if (std::holds_alternative<std::string>(cond.value)) {
				const std::string& literalValue = std::get<std::string>(cond.value);
				switch (cond.opType) {
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
		catch (const std::exception& e) { // Łapie błędy konwersji (np. stol) lub inne
			std::cerr << "Warning: Type conversion or comparison error in WHERE for value '" << columnValueStr << "': " << e.what() << std::endl;
			return false; // Błąd = warunek niespełniony
		}
	}

	std::unique_ptr<MiniDb::Table::QueryResult> SelectStatement::execute(MiniDb::Database::Database& database) const {
		if (!_statement || !_statement->fromTable) {
			throw std::runtime_error("Invalid SELECT statement structure: missing FROM clause.");
		}

		// 1. Przygotuj struktury do przechowywania informacji o zapytaniu
		std::vector<QueryTableInfo> queryTablesOrder; // Kolejność tabel jak w zapytaniu (FROM, JOIN1, JOIN2...)
		std::map<std::string, size_t> aliasToIndex; // Mapowanie aliasu na indeks w queryTablesOrder
		std::map<std::string, const MiniDb::Table::Columns*> tableColumnsMap; // Cache definicji kolumn per alias

		// 2. Rekurencyjne przetwarzanie FROM/JOIN
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
				aliasToIndex[tableAlias] = currentIndex;
				// Dodajemy tabelę bez warunku JOIN (bo to albo tabela bazowa, albo prawa strona JOINa,
				// której warunek zostanie dodany później w sekcji kTableJoin)
				queryTablesOrder.push_back({ tableAlias, tableName, std::move(table), std::nullopt });
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

		// 3. Przetwórz listę SELECT
		MiniDb::Table::Columns resultColumnsDefinition; // Definicje kolumn dla wyniku
		std::vector<SelectedColumnInfo> selectedColumnsList; // Co dokładnie wybrać i skąd

		if (!_statement->selectList) {
			throw std::runtime_error("Missing SELECT list.");
		}

		bool selectAll = _statement->selectList->size() == 1 && (*_statement->selectList)[0]->type == hsql::kExprStar;

		if (selectAll) {
			for (const auto& qtInfo : queryTablesOrder) {
				for (const auto& colDef : qtInfo.table.columns.getColumns()) {
					// Tworzymy nową definicję dla wyniku, potencjalnie z prefiksem aliasu dla jasności
					MiniDb::Table::Column resultColDef = colDef; // Kopiujemy metadane
					resultColDef.name = qtInfo.alias + "." + colDef.name; // Np. "u.login"
					resultColumnsDefinition.addColumn(resultColDef);
					selectedColumnsList.push_back({ qtInfo.alias, colDef.name, colDef }); // Przechowujemy oryginał
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

					auto aliasIt = aliasToIndex.find(tableAlias);
					if (aliasIt == aliasToIndex.end()) {
						throw std::runtime_error("Unknown table alias '" + tableAlias + "' in SELECT list.");
					}

					size_t tableIndex = aliasIt->second;
					const MiniDb::Table::Table& sourceTable = queryTablesOrder[tableIndex].table;

					try {
						const MiniDb::Table::Column& originalColDef = sourceTable.columns.getColumnByName(columnName);
						resultColumnsDefinition.addColumn(originalColDef); // Dodajemy do definicji wyniku
						selectedColumnsList.push_back({ tableAlias, columnName, originalColDef }); // Zapisujemy skąd brać dane
					}
					catch (const std::runtime_error& e) {
						throw std::runtime_error("Column '" + columnName + "' not found in table with alias '" + tableAlias + "'.");
					}
				}
				else {
					throw std::runtime_error("Unsupported expression type in SELECT list. Only columns (alias.name) or * are supported.");
				}
			}
		}

		// 4. Przetwórz klauzulę WHERE
		std::optional<ParsedWhereCondition> whereConditionOpt;

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
				if (!aliasToIndex.count(tableAlias)) {
					throw std::runtime_error("Unknown table alias '" + tableAlias + "' in WHERE clause.");
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

				whereConditionOpt.emplace(ParsedWhereCondition{ tableAlias, colName, clause->opType, literal });

			}
			else {
				throw std::runtime_error("Unsupported WHERE clause structure. Expected: alias.column OPERATOR literal.");
			}
		}

		// 5. Wykonaj złączenie (Nested Loop) i filtrowanie
		auto queryResult = std::make_unique<MiniDb::Table::QueryResult>();
		queryResult->columns = resultColumnsDefinition; // Ustaw metadane kolumn wyniku

		// Rekurencyjna funkcja pomocnicza do iteracji
		std::function<void(size_t, JoinedRowContext)> processJoinLevel =
			[&](size_t currentTableIndex, JoinedRowContext currentContext)
			{
				const QueryTableInfo& currentTableInfo = queryTablesOrder[currentTableIndex];

				for (const MiniDb::Table::Row& row : currentTableInfo.table.rows.getRows()) {
					currentContext[currentTableInfo.alias] = &row; // Aktualizujemy kontekst

					bool joinOk = true;
					if (currentTableIndex > 0) { // Sprawdź warunek JOIN dla tabel > 0
						// joinCondition powinno być ustawione w QueryTableInfo dla tabel > 0
						if (!currentTableInfo.joinCondition.has_value()) {
							throw std::logic_error("Internal error: Missing JOIN condition for table " + currentTableInfo.alias);
						}
						joinOk = evaluateJoinCondition(currentTableInfo.joinCondition.value(), currentContext, tableColumnsMap);
					}

					if (joinOk) {
						if (currentTableIndex + 1 < queryTablesOrder.size()) {
							// Przejdź do następnej tabeli w JOIN
							processJoinLevel(currentTableIndex + 1, currentContext);
						}
						else {
							// Ostatnia tabela - mamy pełny połączony wiersz (w currentContext)
							// Zastosuj WHERE
							bool whereOk = true;
							if (whereConditionOpt) {
								whereOk = evaluateWhereCondition(whereConditionOpt.value(), currentContext, tableColumnsMap);
							}

							if (whereOk) {
								// Zbuduj wiersz wynikowy
								std::vector<std::string> resultRowValues;
								resultRowValues.reserve(selectedColumnsList.size());
								for (const auto& selectedCol : selectedColumnsList) {
									resultRowValues.push_back(
										getValueFromContext(currentContext, selectedCol.sourceTableAlias, selectedCol.sourceColumnName, tableColumnsMap)
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
		if (!queryTablesOrder.empty()) {
			JoinedRowContext initialContext;
			processJoinLevel(0, initialContext);
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
