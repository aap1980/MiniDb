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

	void Database::createTable(const std::string& _tableName, const std::vector<MiniDb::Table::Column>& columns) {
		if (tables.find(_tableName) != tables.end()) {
			throw std::runtime_error("Table '" + _tableName + "' already exists.");
		}

		MiniDb::Table::Table table(_tableName);
		table.metadata.addColumns(columns);
		table.metadata.saveToFile();
		table.saveToFile();

		tables.emplace(_tableName, std::move(table));
	}

	bool Database::loadAllTables() {
		namespace fs = std::filesystem;
		std::string tablesPath = MiniDb::Config::Config::getInstance().getTablesPath();
		for (const auto& entry : fs::directory_iterator(tablesPath)) {
			if (entry.path().extension() == ".md") {
				std::string _tableName = entry.path().stem().string();
				MiniDb::Table::Table table(_tableName);
				tables.emplace(_tableName, std::move(table));
				std::cout << "Successfully loaded table: " << _tableName << "\n";
			}
		}

		return !tables.empty();
	}

	MiniDb::Table::Table& Database::getTable(const std::string& _tableName) {
		auto it = tables.find(_tableName);
		if (it != tables.end()) {
			return it->second;
		}
		throw std::runtime_error("Table not found: " + _tableName);
	}

	void Database::listTables() const {
		for (const auto& [name, _] : tables) {
			std::cout << "- " << name << "\n";
		}
	}

}
