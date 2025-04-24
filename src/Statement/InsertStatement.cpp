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
		const MiniDb::Table::Columns& columns = table.columns;

		// Przygotuj wektor na wartoœci nowego wiersza
		std::vector<std::string> rowValues;
		rowValues.resize(columns.size(), "");

		const std::vector<hsql::Expr*>* insertValues = _statement->values;

		if (_statement->columns != nullptr) {
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

			// Mapuj podane wartoœci na odpowiednie pozycje w `rowValues`
			std::set<size_t> filledIndices; // Œledzenie, które kolumny tabeli zosta³y wype³nione
			for (size_t i = 0; i < numTargetColumns; ++i) {
				const char* targetColName = targetColumnNames[i];
				if (!targetColName) { throw std::runtime_error("Internal error: null column name in INSERT list."); }

				const hsql::Expr* valueExpr = (*insertValues)[i];
				std::string valueStr = valueExprToString(valueExpr);
				bool isNull = isValueExprNull(valueExpr);

				try {
					// ZnajdŸ indeks i definicjê kolumny w tabeli docelowej
					size_t tableColIndex = columns.getColumnIndexByName(targetColName);
					const MiniDb::Table::Column& colDef = columns.getColumns()[tableColIndex];

					// SprawdŸ NOT NULL constraint
					if (isNull && colDef.isNotNull()) {
						throw std::runtime_error("Cannot insert NULL into non-nullable column '" + std::string(targetColName) + "'.");
					}

					// TODO: Walidacja typu (np. czy valueStr pasuje do colDef.type) - pomijamy na razie

					// Przypisz wartoœæ do odpowiedniego miejsca w wektorze wiersza
					rowValues[tableColIndex] = valueStr;
					filledIndices.insert(tableColIndex);

				}
				catch (const std::runtime_error& e) { // B³¹d z getColumnIndexByName
					throw std::runtime_error("Column '" + std::string(targetColName) + "' specified in INSERT list not found in table '" + tableName + "'.");
				}
			}

			// SprawdŸ, czy nie pominiêto kolumn NOT NULL (które nie maj¹ wartoœci domyœlnej)
			for (size_t i = 0; i < columns.size(); ++i) {
				// Jeœli indeksu nie ma w 'filledIndices' ORAZ kolumna jest NOT NULL
				if (filledIndices.find(i) == filledIndices.end() && columns.getColumns()[i].isNotNull()) {
					// TODO: W przysz³oœci mo¿na by tu sprawdzaæ, czy kolumna ma wartoœæ DOMYŒLN¥
					throw std::runtime_error("Non-nullable column '" + columns.getColumns()[i].name + "' must be specified in INSERT statement or have a default value.");
				}
			}

		}
		else {
			throw std::runtime_error("INSERT statement must specify target column list explicitly.");
		}

		MiniDb::Table::Row newRow(std::move(rowValues));

		try {
			//database.appendRowToTable(tableName, newRow); // Potrzebna implementacja tej metody w Database
		}
		catch (const std::exception& e) {
			throw std::runtime_error("Failed to insert row into table '" + tableName + "': " + e.what());
		}

		std::cout << "Row inserted successfully into table '" << tableName << "'." << std::endl;
	}

}