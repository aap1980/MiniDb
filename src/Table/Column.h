#pragma once
#include <string>

namespace MiniDb::Table {

	class Column {
	public:
		std::string name;
		std::string type;
		bool nullable; // true = NULL, false = NOT NULL

		Column(const std::string& name, const std::string& type, bool nullable = true);

		bool isNullable() const { return nullable; }
		bool isNotNull()  const { return !nullable; }
	};

}
