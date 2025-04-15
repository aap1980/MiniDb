#include <filesystem>
#include <iostream>
#include <stdexcept>
#include "Database.h"
#include "../Table/Table.h"
#include "../Config/Config.h"

namespace MiniDb::Database {

	Database& Database::getInstance() {
		static Database instance;
		return instance;
	}

	void Database::createTable(const std::string& tableName, const std::vector<MiniDb::Table::Column>& columns) {
		if (tables.find(tableName) != tables.end()) {
			throw std::runtime_error("Table '" + tableName + "' already exists.");
		}

		MiniDb::Table::Table table(tableName);
		table.columns.addColumns(columns);
		table.saveMetadataToFile();

		tables.emplace(tableName, std::move(table));
	}

	void Database::loadTables() {
		std::string tablesPath = MiniDb::Config::Config::getInstance().getTablesPath();
		for (const auto& entry : std::filesystem::directory_iterator(tablesPath)) {
			if (entry.path().extension() == ".md") {
				std::string _tableName = entry.path().stem().string();
				MiniDb::Table::Table table(_tableName);
				tables.emplace(_tableName, std::move(table));
				std::cout << "Successfully loaded table: " << _tableName << "\n";
			}
		}
	}

	MiniDb::Table::Table& Database::getTable(const std::string& tableName) {
		auto it = tables.find(tableName);
		if (it != tables.end()) {
			return it->second;
		}
		throw std::runtime_error("Table not found: " + tableName);
	}

}
