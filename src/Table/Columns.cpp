#include "Columns.h"
#include <stdexcept>

namespace MiniDb::Table {

	std::size_t Columns::size() const {
		return _columns.size();
	}

	void Columns::addColumn(const Column& column) {
		for (const auto& col : _columns) {
			if (col.name == column.name) {
				throw std::runtime_error("Duplicate column name: " + column.name);
			}
		}
		_columns.push_back(column);
	}

	void Columns::addColumns(const std::vector<Column>& columns) {
		for (const auto& column : columns) {
			addColumn(column);
		}
	}

	const std::vector<Column>& Columns::getColumns() const {
		return _columns;
	}

	const Column& Columns::getColumnByName(const std::string& name) const {
		for (const auto& column : _columns) {
			if (column.name == name) {
				return column;
			}
		}
		throw std::out_of_range("Column not found: " + name);
	}

	const std::size_t Columns::getColumnIndexByName(const std::string& name) const {
		for (std::size_t i = 0; i < _columns.size(); ++i) {
			if (_columns[i].name == name) {
				return i;
			}
		}
		throw std::out_of_range("Column not found: " + name);
	}

}