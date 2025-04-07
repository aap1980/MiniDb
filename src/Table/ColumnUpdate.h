#pragma once
#include <string>

namespace MiniDb::Table {

	class ColumnUpdate {
	private:
		std::string name;
		std::string value;
	public:
		ColumnUpdate(const std::string& name, const std::string& value);
		const std::string& getName() const;
		const std::string& getValue() const;
	};

}
