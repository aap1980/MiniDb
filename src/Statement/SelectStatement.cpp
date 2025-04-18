#include "SelectStatement.h"
#include "../Database/Database.h"
#include "../SqlParser/SQLParser.h"
#include "../Table/QueryResult.h"
#include "../Table/Columns.h"
#include <iostream>
#include <optional>
#include <functional>

namespace MiniDb::Statement {

	SelectStatement::SelectStatement(std::unique_ptr<hsql::SQLParserResult> parserResult) {
		_parserResult = std::move(parserResult);
		_statement = static_cast<const hsql::SelectStatement*>(_parserResult->getStatement(0));
	}

	std::unique_ptr<MiniDb::Table::QueryResult> SelectStatement::execute(MiniDb::Database::Database& database) const {
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
