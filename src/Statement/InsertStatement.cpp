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

		MiniDb::Table::Table& table = database.getTable(tableName);
		table.loadDataFromFile();
		const MiniDb::Table::Columns& columns = table.columns;

		MiniDb::Table::Row row(columns.size());

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

			// Mapuj podane wartoœci na odpowiednie pozycje w `row`
			for (size_t i = 0; i < numTargetColumns; ++i) {
				const char* targetColumnName = targetColumnNames[i];
				if (!targetColumnName) { throw std::runtime_error("Internal error: null column name in INSERT list."); }

				const hsql::Expr* valueExpr = (*insertValues)[i];
				std::string valueString = valueExprToString(valueExpr);
				bool isNull = isValueExprNull(valueExpr);

				size_t tableColumnIndex = columns.getColumnIndexByName(targetColumnName);
				const MiniDb::Table::Column& column = columns.getColumns()[tableColumnIndex];

				if (isNull && column.isNotNull()) {
					throw std::runtime_error("Cannot insert NULL into non-nullable column '" + std::string(targetColumnName) + "'.");
				}

				row.setValueByIndex(tableColumnIndex, valueString);
			}

			// Czy nie pominiêto kolumn NOT NULL, które nie maj¹ wartoœci domyœlnej
			for (size_t i = 0; i < columns.size(); ++i) {
				const MiniDb::Table::Column& colDef = columns.getColumns()[i];
				const std::string& value = row.getValueByIndex(i);
				if (value.empty() && colDef.isNotNull()) {
					throw std::runtime_error("Non-nullable column '" + colDef.name + "' must be specified in INSERT statement or have a default value.");
				}
			}

		}
		else {
			throw std::runtime_error("INSERT statement must specify target column list explicitly.");
		}

		table.rows.addRow(row);
		table.saveDataToFile();
	}

}