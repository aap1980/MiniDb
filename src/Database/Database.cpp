#include <filesystem>
#include <iostream>
#include "Database.h"
#include "../Table/Table.h"
using namespace MiniDb::Table;

namespace MiniDb::Database {

	bool Database::loadAllTables(const std::string& path) {
		namespace fs = std::filesystem;

		for (const auto& entry : fs::directory_iterator(path)) {
			if (entry.path().extension() == ".md") {
				std::string tableName = entry.path().stem().string();

				Table table;
				if (table.loadMetadata(path + tableName + ".md") &&
					table.loadData(path + tableName + ".dat")) {
					tables.emplace(tableName, std::move(table));
				}
				else {
					std::cerr << "Nie uda³o siê za³adowaæ tabeli: " << tableName << "\n";
				}
			}
		}

		return !tables.empty();
	}

	Table* Database::getTable(const std::string& tableName) {
		auto it = tables.find(tableName);
		return it != tables.end() ? &it->second : nullptr;
	}

	void Database::listTables() const {
		for (const auto& [name, _] : tables) {
			std::cout << "- " << name << "\n";
		}
	}

}
