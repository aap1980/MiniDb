#include <filesystem>
#include <iostream>
#include "Database.h"
#include "../Table/Table.h"
#include "../Config/Config.h"

namespace MiniDb::Database {

	bool Database::loadAllTables() {
		namespace fs = std::filesystem;
		std::string tablesPath = MiniDb::Config::Config::getInstance().getTablesPath();
		for (const auto& entry : fs::directory_iterator(tablesPath)) {
			if (entry.path().extension() == ".md") {
				std::string tableName = entry.path().stem().string();
				MiniDb::Table::Table table(tableName);
				tables.emplace(tableName, std::move(table));
				std::cout << "Successfully loaded table: " << tableName << "\n";
			}
		}

		return !tables.empty();
	}

	MiniDb::Table::Table& Database::getTable(const std::string& tableName) {
		auto it = tables.find(tableName);
		if (it != tables.end()) {
			return it->second;
		}
		throw std::runtime_error("Table not found: " + tableName);
	}

	void Database::listTables() const {
		for (const auto& [name, _] : tables) {
			std::cout << "- " << name << "\n";
		}
	}

}
