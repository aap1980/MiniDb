#pragma once

#include "QueryTablesOrder.h"
#include "QueryTableInfo.h"
#include <vector>

namespace MiniDb::Statement {

	class QueryTablesOrder {
	public:
		std::vector<MiniDb::Statement::QueryTableInfo> queryTableInfos;
		std::size_t size() const;
		void addQueryTableInfo(const MiniDb::Statement::QueryTableInfo& queryTableInfo);
		bool hasAlias(const std::string& alias) const;
		MiniDb::Statement::QueryTableInfo& getByIndex(std::size_t index);
		MiniDb::Statement::QueryTableInfo& getByAlias(const std::string& alias);
		MiniDb::Statement::QueryTableInfo& last();
	};

}
