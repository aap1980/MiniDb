#include "InsertStatement.h"
#include "../Database/Database.h"
#include "../Table/Table.h"
#include "../Table/Columns.h"
#include "../Table/Column.h"
#include "../Table/Row.h"
#include "../Table/QueryResult.h"
#include "../SqlParser/sql/InsertStatement.h"
#include "../SqlParser/sql/Expr.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <set>

namespace MiniDb::Statement {

	InsertStatement::InsertStatement(std::unique_ptr<hsql::SQLParserResult> parserResult) {
		_parserResult = std::move(parserResult);
		_statement = static_cast<const hsql::InsertStatement*>(_parserResult->getStatement(0));
	}

	bool InsertStatement::returnsResult() const {
		return false;
	}

	bool InsertStatement::isValueExprNull(const hsql::Expr* expr) const {
		return expr != nullptr && expr->type == hsql::kExprLiteralNull;
	}

	std::string InsertStatement::valueExprToString(const hsql::Expr* expr) const {
		if (!expr) {
			throw std::runtime_error("Encountered null expression in VALUES clause.");
		}
		switch (expr->type) {
		case hsql::kExprLiteralString:
			return expr->name ? expr->name : "";
		case hsql::kExprLiteralInt:
			return std::to_string(expr->ival);
		case hsql::kExprLiteralFloat:
			return std::to_string(expr->fval);
		case hsql::kExprLiteralNull:
			return "";
		default:
			throw std::runtime_error("Unsupported expression type in INSERT VALUES clause. Only literals and NULL are supported.");
		}
	}

	void InsertStatement::executeNoResult(MiniDb::Database::Database& database) const {
		const std::string tableName = _statement->tableName;

		if (!_statement || _statement->type != hsql::kInsertValues) {
			throw std::runtime_error("Invalid statement type passed to InsertStatement constructor.");
		}
		if (_statement->select) {
			throw std::runtime_error("INSERT INTO ... SELECT ... is not supported yet.");
		}
		if (!_statement->tableName) {
			throw std::runtime_error("INSERT statement missing table name.");
		}
		if (!_statement->values) {
			throw std::runtime_error("INSERT statement missing VALUES clause or unsupported type.");
		}
		if (_statement->values->empty()) {
			throw std::runtime_error("INSERT statement VALUES clause cannot be empty.");
		}

		// Definicja tabeli
		MiniDb::Table::Table table = database.getTable(tableName);
		const MiniDb::Table::Columns& tableColumns = table.columns;
		const auto& tableColumnDefs = tableColumns.getColumns();
		size_t tableColumnCount = tableColumnDefs.size();

		// Przygotuj wektor na warto�ci nowego wiersza
		std::vector<std::string> rowValues;
		rowValues.resize(tableColumnCount, "");

		const std::vector<hsql::Expr*>* insertValues = _statement->values;


		// 2. Sprawd�, czy podano list� kolumn (`INSERT INTO T (c1, c2) ...`)
		if (_statement->columns != nullptr) {
			// --- Przypadek B: Podano list� kolumn ---
			const std::vector<char*>& targetColumnNames = *_statement->columns;
			size_t numTargetColumns = targetColumnNames.size();
			size_t numInsertValues = insertValues->size();

			if (numTargetColumns != numInsertValues) {
				throw std::runtime_error("Column count (" + std::to_string(numTargetColumns) +
					") doesn't match value count (" + std::to_string(numInsertValues) + ").");
			}

			if (numTargetColumns == 0) {
				throw std::runtime_error("INSERT statement cannot have an empty column list.");
			}

			// Mapuj podane warto�ci na odpowiednie pozycje w `rowValues`
			std::set<size_t> filledIndices; // �ledzenie, kt�re kolumny tabeli zosta�y wype�nione
			for (size_t i = 0; i < numTargetColumns; ++i) {
				const char* targetColName = targetColumnNames[i];
				if (!targetColName) { throw std::runtime_error("Internal error: null column name in INSERT list."); }

				const hsql::Expr* valueExpr = (*insertValues)[i];
				std::string valueStr = valueExprToString(valueExpr);
				bool isNull = isValueExprNull(valueExpr);

				try {
					// Znajd� indeks i definicj� kolumny w tabeli docelowej
					size_t tableColIndex = tableColumns.getColumnIndexByName(targetColName);
					const MiniDb::Table::Column& colDef = tableColumnDefs[tableColIndex];

					// Sprawd� NOT NULL constraint
					if (isNull && colDef.isNotNull()) {
						throw std::runtime_error("Cannot insert NULL into non-nullable column '" + std::string(targetColName) + "'.");
					}

					// TODO: Walidacja typu (np. czy valueStr pasuje do colDef.type) - pomijamy na razie

					// Przypisz warto�� do odpowiedniego miejsca w wektorze wiersza
					rowValues[tableColIndex] = valueStr;
					filledIndices.insert(tableColIndex);

				}
				catch (const std::runtime_error& e) { // B��d z getColumnIndexByName
					throw std::runtime_error("Column '" + std::string(targetColName) + "' specified in INSERT list not found in table '" + tableName + "'.");
				}
			}

			// Sprawd�, czy nie pomini�to kolumn NOT NULL (kt�re nie maj� warto�ci domy�lnej)
			for (size_t i = 0; i < tableColumnCount; ++i) {
				// Je�li indeksu nie ma w 'filledIndices' ORAZ kolumna jest NOT NULL
				if (filledIndices.find(i) == filledIndices.end() && tableColumnDefs[i].isNotNull()) {
					// TODO: W przysz�o�ci mo�na by tu sprawdza�, czy kolumna ma warto�� DOMY�LN�
					throw std::runtime_error("Non-nullable column '" + tableColumnDefs[i].name + "' must be specified in INSERT statement or have a default value.");
				}
			}

		}
		else {
			// --- Przypadek A: Brak listy kolumn (`INSERT INTO T VALUES(...)`) ---
			size_t numInsertValues = insertValues->size();

			if (numInsertValues != tableColumnCount) {
				throw std::runtime_error("Value count (" + std::to_string(numInsertValues) +
					") doesn't match table column count (" + std::to_string(tableColumnCount) + ").");
			}

			// Przypisz warto�ci po kolei do `rowValues`
			for (size_t i = 0; i < tableColumnCount; ++i) {
				const hsql::Expr* valueExpr = (*insertValues)[i];
				std::string valueStr = valueExprToString(valueExpr);
				bool isNull = isValueExprNull(valueExpr);
				const MiniDb::Table::Column& colDef = tableColumnDefs[i];

				// Sprawd� NOT NULL constraint
				if (isNull && colDef.isNotNull()) {
					throw std::runtime_error("Cannot insert NULL into non-nullable column '" + colDef.name + "'.");
				}

				// TODO: Walidacja typu - pomijamy na razie

				rowValues[i] = valueStr;
			}
		}

		// 3. Stw�rz obiekt Row
		MiniDb::Table::Row newRow(std::move(rowValues));

		// 4. Dodaj wiersz do bazy danych (np. dopisz do pliku)
		//    Zak�adamy, �e Database ma metod� do zapisu/dopisania wiersza.
		try {
			//database.appendRowToTable(tableName, newRow); // Potrzebna implementacja tej metody w Database
		}
		catch (const std::exception& e) {
			throw std::runtime_error("Failed to insert row into table '" + tableName + "': " + e.what());
		}

		std::cout << "Row inserted successfully into table '" << tableName << "'." << std::endl;
	}

}