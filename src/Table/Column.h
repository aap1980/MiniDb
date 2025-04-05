#pragma once
#include <string>

namespace MiniDb::Table {

    class Column {
    public:
        std::string name;
        std::string type;

        Column(const std::string& name, const std::string& type);
    };

}
