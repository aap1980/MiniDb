#include "Row.h"
#include <stdexcept>

namespace MiniDb::Table {

	Row::Row(std::size_t count, const std::string& defaultValue)
		: _values(count, defaultValue) {
	}

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

	void Row::setValueByIndex(std::size_t index, const std::string& value) {
		if (index >= _values.size()) {
			throw std::out_of_range("Row::setValueByIndex: index out of range");
		}
		_values[index] = value;
	}

}
