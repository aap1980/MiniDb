#pragma once

#include <string>
#include "../SqlParser/SQLParser.h"

namespace MiniDb::Utils {

	class SqlUtils {
	public:
		static std::string dataTypeToString(hsql::DataType dataType);
	};

}
