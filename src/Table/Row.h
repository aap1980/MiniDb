#pragma once

#include <vector>
#include <string>

namespace MiniDb::Table {

	class Row {
	public:
		Row() = default;
		explicit Row(std::vector<std::string> values);
		std::size_t size() const;
		void addValue(const std::string& value);
		const std::vector<std::string>& getValues() const;
		const std::string& getValueByIndex(std::size_t index) const;
	private:
		std::vector<std::string> _values;
	};

}
