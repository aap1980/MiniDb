#include "Config.h"
#include <iostream>
#include <fstream>

namespace MiniDb::Config {

    bool Config::loadConfig(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "B��d otwarcia pliku konfiguracyjnego: " << filename << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.find("TablesPath") != std::string::npos) {
                tablesPath = line.substr(line.find("=") + 1);
            }
        }

        file.close();
        return true;
    }

}
