#include <iostream>
#include <fstream>
#include <filesystem>
#include "Config.h"

namespace MiniDb::Config {

	Config& Config::getInstance() {
		static Config instance;
		return instance;
	}

	bool Config::loadConfig(const std::string& filename) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			std::cerr << "Error opening configuration file: " << filename << std::endl;
			return false;
		}

		std::string line;
		while (std::getline(file, line)) {
			if (line.find("TablesPath") != std::string::npos) {
				tablesPath = line.substr(line.find("=") + 1);
				tablesPath = std::filesystem::absolute(tablesPath).string();
			}
		}

		file.close();
		return true;
	}

	const std::string& Config::getTablesPath() const {
		return tablesPath;
	}

}
