#pragma once
#include <string>

namespace MiniDb::Config {

    class Config {
    public:
        std::string tablesPath;

        bool loadConfig(const std::string& filename);
    };

}
