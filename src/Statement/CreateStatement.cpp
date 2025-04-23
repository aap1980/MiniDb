#include "CreateStatement.h"
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

	CreateStatement::CreateStatement(std::unique_ptr<hsql::SQLParserResult> parserResult) {
		_parserResult = std::move(parserResult);
		_statement = static_cast<const hsql::CreateStatement*>(_parserResult->getStatement(0));
	}

	std::unique_ptr<MiniDb::Table::QueryResult> CreateStatement::execute(MiniDb::Database::Database& database) const {
		std::string tableName = _statement->tableName;
		bool ifNotExists = _statement->ifNotExists;

		if (!_statement || _statement->type != hsql::kCreateTable) {
			throw std::runtime_error("Internal error: Invalid statement type passed to CreateStatement constructor.");
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
				std::cout << "Notice: Table '" << tableName << "' already exists, skipping creation (IF NOT EXISTS specified)." << std::endl;
				// Zwróæ pusty, ale wa¿ny obiekt QueryResult, zgodnie z sygnatur¹ metody bazowej
				return std::make_unique<MiniDb::Table::QueryResult>();
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

		for (const hsql::ColumnDefinition* hsqlColDef : *_statement->columns) {
			if (!hsqlColDef || !hsqlColDef->name) {
				throw std::runtime_error("Invalid column definition in CREATE TABLE statement.");
			}
			std::string colName = hsqlColDef->name;
			if (!columnNames.insert(colName).second) {
				throw std::runtime_error("Duplicate column name '" + colName + "' in CREATE TABLE statement.");
			}
			std::string columnType = MiniDb::Utils::SqlUtils::dataTypeToString(hsqlColDef->type.data_type);
			bool isNotNull = !hsqlColDef->nullable;
			MiniDb::Table::Column column(colName, columnType);
			columns.addColumn(column);
		}

		if (columns.getColumns().empty()) {
			throw std::runtime_error("CREATE TABLE must define at least one column.");
		}

		// Tworzymy tabele
		try {
			database.createTable(tableName, columns);
		}
		catch (const std::exception& e) {
			throw std::runtime_error("Failed to register table '" + tableName + "': " + e.what());
		}

		std::cout << "Table '" << tableName << "' created successfully." << std::endl;

		// Zwróæ pusty, ale wa¿ny obiekt QueryResult, zgodnie z sygnatur¹ metody bazowej
		return std::make_unique<MiniDb::Table::QueryResult>();
	}

}
