#include "CreateTableStatement.h"
#include "../Database/Database.h"
#include "../Table/Columns.h"
#include "../Table/Column.h"
#include "../Table/QueryResult.h"
#include "../SqlParser/sql/CreateStatement.h"
#include "../Utils/SqlUtils.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <set>
#include <iostream>
#include <memory>

namespace MiniDb::Statement {

	CreateTableStatement::CreateTableStatement(std::unique_ptr<hsql::SQLParserResult> parserResult) {
		_parserResult = std::move(parserResult);
		_statement = static_cast<const hsql::CreateStatement*>(_parserResult->getStatement(0));
	}

	bool CreateTableStatement::returnsResult() const {
		return false;
	}

	void CreateTableStatement::executeNoResult(MiniDb::Database::Database& database) const {
		std::string tableName = _statement->tableName;
		bool ifNotExists = _statement->ifNotExists;

		if (!_statement || _statement->type != hsql::kCreateTable) {
			throw std::runtime_error("Invalid statement type passed to CreateStatement constructor.");
		}
		if (!_statement->tableName) {
			throw std::runtime_error("CREATE TABLE statement missing table name.");
		}
		if (!_statement->columns && !_statement->select) {
			throw std::runtime_error("CREATE TABLE statement missing column definitions.");
		}
		if (_statement->select) {
			throw std::runtime_error("CREATE TABLE ... AS SELECT is not supported yet.");
		}

		// Sprawdzamy istnienie tabeli
		if (database.tableExists(tableName)) {
			if (ifNotExists) {
				std::cout << "Table '" << tableName << "' already exists, skipping creation." << std::endl;
			}
			else {
				throw std::runtime_error("Table '" + tableName + "' already exists.");
			}
		}

		// Definicja kolumn
		MiniDb::Table::Columns columns;
		std::set<std::string> columnNames;

		if (!_statement->columns) {
			throw std::runtime_error("Missing column definitions in CREATE TABLE statement.");
		}

		for (const hsql::ColumnDefinition* columnDefinition : *_statement->columns) {
			if (!columnDefinition || !columnDefinition->name) {
				throw std::runtime_error("Invalid column definition in CREATE TABLE statement.");
			}
			std::string columName = columnDefinition->name;
			if (!columnNames.insert(columName).second) {
				throw std::runtime_error("Duplicate column name '" + columName + "' in CREATE TABLE statement.");
			}
			std::string columnType = MiniDb::Utils::SqlUtils::dataTypeToString(columnDefinition->type.data_type);
			bool isNotNull = !columnDefinition->nullable;
			MiniDb::Table::Column column(columName, columnType);
			columns.addColumn(column);
		}

		if (columns.getColumns().empty()) {
			throw std::runtime_error("CREATE TABLE must define at least one column.");
		}

		database.createTable(tableName, columns);
	}

}
