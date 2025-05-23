#pragma once
#include <unordered_map>
#include <string>
#include "../Table/Table.h"

namespace MiniDb::Database {

	class Database {
	private:
		std::unordered_map<std::string, MiniDb::Table::Table> tables;
		Database() = default;
	public:
		static Database& getInstance();
		void createTable(const std::string& tableName, const MiniDb::Table::Columns& columns);
		void loadTables();
		bool tableExists(const std::string& tableName) const;
		MiniDb::Table::Table& getTable(const std::string& tableName);
	};

}