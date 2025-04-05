#include <filesystem>
#include <iostream>
#include "Database.h"
#include "../Table/Table.h"

namespace MiniDb::Database {

	bool Database::loadAllTables(const std::string& path) {
		namespace fs = std::filesystem;

		for (const auto& entry : fs::directory_iterator(path)) {
			if (entry.path().extension() == ".md") {
				std::string tableName = entry.path().stem().string();

				MiniDb::Table::Table table(tableName);
				if (table.loadMetadata(path + tableName + ".md") &&
					table.loadData(path + tableName + ".dat")) {
					tables.emplace(tableName, std::move(table));
				}
				else {
					std::cerr << "Nie udało się załadować tabeli: " << tableName << "\n";
				}
			}
		}

		return !tables.empty();
	}

	MiniDb::Table::Table* Database::getTable(const std::string& tableName) {
		auto it = tables.find(tableName);
		return it != tables.end() ? &it->second : nullptr;
	}

	void Database::listTables() const {
		for (const auto& [name, _] : tables) {
			std::cout << "- " << name << "\n";
		}
	}

}
