#pragma once

#include <vector>
#include <string>

namespace MiniDb::Table {

	class Row {
	public:
		Row() = default;
		Row(std::size_t count, const std::string& defaultValue = "");
		explicit Row(std::vector<std::string> values);
		std::size_t size() const;
		void addValue(const std::string& value);
		const std::vector<std::string>& getValues() const;
		const std::string& getValueByIndex(std::size_t index) const;
		void setValueByIndex(std::size_t index, const std::string& value);
	private:
		std::vector<std::string> _values;
	};

}
