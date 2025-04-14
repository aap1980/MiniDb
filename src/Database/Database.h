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
		void createTable(const std::string& _tableName, const std::vector<MiniDb::Table::Column>& columns);
		bool loadAllTables();
		MiniDb::Table::Table& getTable(const std::string& _tableName);
		void listTables() const;
	};

}