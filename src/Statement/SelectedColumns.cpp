#include "SelectedColumns.h"

namespace MiniDb::Statement {

	std::size_t SelectedColumns::size() const {
		return columns.size();
	}

	void SelectedColumns::addColumn(const MiniDb::Statement::SelectedColumnInfo& column) {
		columns.push_back(column);
	}

}
