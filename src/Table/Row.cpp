#include "Row.h"

namespace MiniDb::Table {

	Row::Row(std::vector<std::string> values)
		: _values(std::move(values)) {
	}

	std::size_t Row::size() const {
		return _values.size();
	}

	void Row::addValue(const std::string& value) {
		_values.push_back(value);
	}

	const std::vector<std::string>& Row::getValues() const {
		return _values;
	}

	const std::string& Row::getValueByIndex(std::size_t index) const {
		return _values.at(index);
	}

}
