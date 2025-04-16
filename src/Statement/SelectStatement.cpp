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

		//if (_statement->fromTable != nullptr)
		//	_tableName = _statement->fromTable->getName();

		//_selectAll = _statement->selectList->size() == 1 &&
		//	(*_statement->selectList)[0]->type == hsql::kExprStar;
	}

	std::unique_ptr<MiniDb::Table::QueryResult> SelectStatement::execute(MiniDb::Database::Database& database) const {
		// 1. Pobierz tabelê
		const std::string tableName = _statement->fromTable->getName();
		const auto& table = database.getTable(tableName);
		const auto& columns = table.columns.getColumns();
		const auto& rows = table.rows.getRows();

		// 2. Okreœl indeksy kolumn do SELECT
		MiniDb::Table::Columns selectedColumns;
		std::vector<std::size_t> selectedColumnIndexes;
		std::vector<std::string> selectedColumnNames;

		for (const auto* expr : *_statement->selectList) {
			if (expr->type == hsql::kExprColumnRef) {
				std::string columnName = expr->name;
				selectedColumns.addColumn(table.columns.getColumnByName(columnName));
				selectedColumnNames.push_back(columnName);

				auto it = std::find_if(columns.begin(), columns.end(),
					[&columnName](const Table::Column& col) {
						return col.name == columnName;
					});

				if (it == columns.end()) {
					throw std::runtime_error("Unknown column: " + columnName);
				}

				selectedColumnIndexes.push_back(std::distance(columns.begin(), it));
			}
			else {
				throw std::runtime_error("Only column references are supported.");
			}
		}

		// 3. WHERE — przygotuj filtr (jeœli jest)
		std::optional<std::function<bool(const Table::Row&)>> rowFilter;

		if (_statement->whereClause != nullptr) {
			const auto* where = _statement->whereClause;

			if (where->type == hsql::kExprOperator &&
				where->opType != hsql::kOpOr && where->opType != hsql::kOpAnd &&
				where->expr && where->expr2 &&
				where->expr->type == hsql::kExprColumnRef &&
				where->expr2->type == hsql::kExprLiteralInt) {

				std::string colName = where->expr->name;
				int literal = where->expr2->ival;

				auto it = std::find_if(columns.begin(), columns.end(),
					[&colName](const Table::Column& col) {
						return col.name == colName;
					});

				if (it == columns.end()) {
					throw std::runtime_error("Unknown column in WHERE: " + colName);
				}

				std::size_t colIndex = std::distance(columns.begin(), it);

				// Tworzymy funkcjê filtruj¹c¹
				switch (where->opType) {
				case hsql::kOpEquals:
					rowFilter = [colIndex, literal](const Table::Row& row) {
						return std::stoi(row.getValueByIndex(colIndex)) == literal;
						};
					break;
				case hsql::kOpGreater:
					rowFilter = [colIndex, literal](const Table::Row& row) {
						return std::stoi(row.getValueByIndex(colIndex)) > literal;
						};
					break;
				case hsql::kOpLess:
					rowFilter = [colIndex, literal](const Table::Row& row) {
						return std::stoi(row.getValueByIndex(colIndex)) < literal;
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

		// 4. Sk³adanie QueryResult
		MiniDb::Table::QueryResult queryResult;
		queryResult.columns = selectedColumns;

		for (const auto& row : rows) {
			if (rowFilter && !(*rowFilter)(row)) {
				continue;
			}

			std::vector<std::string> newRow;
			for (std::size_t idx : selectedColumnIndexes) {
				newRow.push_back(row.getValueByIndex(idx));
			}
			queryResult.rows.addRow(Table::Row(std::move(newRow)));
		}

		return std::make_unique<MiniDb::Table::QueryResult>(std::move(queryResult));




		//if (!_selectAll) {
		//	std::cout << "Obs³uga innych kolumn ni¿ * niezaimplementowana.\n";
		//	return;
		//}

		//MiniDb::Table::Table& table = database.getTable(_tableName);
		//table.loadDataFromFile();
		//table.printTable();

		//const auto& columns = table.columns.getColumns();
		//const auto& rows = table.rows;

		//for (const auto& col : columns)
		//	std::cout << col.name << "\t";
		//std::cout << "\n";

		//for (const auto& row : rows) {
		//	for (const auto& val : row)
		//		std::cout << val << "\t";
		//	std::cout << "\n";
		//}
	}

}
