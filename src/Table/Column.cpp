#include "Column.h"

namespace MiniDb::Table {

    Column::Column(const std::string& name, const std::string& type)
        : name(name), type(type) {
    }

}
