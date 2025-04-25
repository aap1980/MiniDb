#include <iostream>
#include <filesystem>
#include "src/Config/Config.h"
#include "src/Database/Database.h"
#include "src/Console/Console.h"

int main() {
	std::filesystem::path current_path = std::filesystem::current_path();

	MiniDb::Config::Config::getInstance().loadConfig("config.ini");

	MiniDb::Database::Database& database = MiniDb::Database::Database::getInstance();

	database.loadTables();

	MiniDb::Console::Console console;

	console.run();

	return 0;
}
