#pragma once

#include "QueryTablesOrder.h"
#include <vector>

namespace MiniDb::Statement {

	class QueryTablesOrder {
	public:
		std::size_t size() const;
		void addColumn(const Column& column);
		void addColumns(const std::vector<Column>& columns);
		const std::vector<Column>& getColumns() const;
		const std::vector<QueryTableInfo>& queryTableInfos;
		const Column& getColumnByName(const std::string& name) const;
		const std::size_t getColumnIndexByName(const std::string& name) const;

	private:
		std::vector<Column> _columns;
	};

}
