#include "ColumnUpdate.h"

namespace MiniDb::Table {

	ColumnUpdate::ColumnUpdate(const std::string& name, const std::string& value)
		: name(name), value(value) {
	}

    const std::string& ColumnUpdate::getName() const {
        return name;
    }

    const std::string& ColumnUpdate::getValue() const {
        return value;
    }

}
