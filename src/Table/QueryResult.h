#pragma once

#include "Row.h"
#include "Columns.h"
#include "Rows.h"

namespace MiniDb::Table {

	class QueryResult {
	public:
		QueryResult() = default;
		Columns columns;
		Rows rows;
	};

}
