#pragma once
#include <string>

namespace MiniDb::Config {

    class Config {
	private:
		std::string tablesPath;
		Config() = default;
    public:
		static Config& getInstance();
        bool loadConfig(const std::string& filename);
		const std::string& getTablesPath() const;
    };

}
