#include "Column.h"

namespace MiniDb::Table {

	Column::Column(const std::string& name, const std::string& type, bool nullable)
		: name(name), type(type), nullable(nullable) {
	}

}
