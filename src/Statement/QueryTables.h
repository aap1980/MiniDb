#pragma once

#include "QueryTables.h"
#include "QueryTable.h"
#include <vector>

namespace MiniDb::Statement {

	class QueryTables {
	public:
		std::vector<MiniDb::Statement::QueryTable> tables;
		std::size_t size() const;
		void addTable(const MiniDb::Statement::QueryTable& table);
		bool hasAlias(const std::string& tableAlias) const;
		MiniDb::Statement::QueryTable& getByIndex(std::size_t index);
		MiniDb::Statement::QueryTable& getByAlias(const std::string& alias);
		MiniDb::Statement::QueryTable& last();
	};

}
