#include <iostream>
#include <filesystem>
#include "src/Config/Config.h"
#include "src/Database/Database.h"
#include "src/Metadata/TableMetadataWriter.h"
#include "src/Table/Table.h"

int main() {
	std::filesystem::path current_path = std::filesystem::current_path();

	MiniDb::Config::Config config;

	if (!config.loadConfig("config.ini")) {
		std::cerr << "B³¹d wczytywania pliku konfiguracyjnego." << std::endl;
		return 1;
	}

	MiniDb::Database::Database database;

	database.loadAllTables(config.tablesPath);

	MiniDb::Table::Table& usersTable = database.getTable("Users");

	usersTable.addRow({ "1", "user1" });
	usersTable.addRow({ "2", "user2" });

	usersTable.printTable();

	/*TableMetadataWriter writer("Users");
	writer.addColumn("id", "int");
	writer.addColumn("login", "string");

	std::filesystem::path file_path = current_path / "Users.md";
	if (writer.saveToFile(file_path.string())) {
		std::cout << "Plik metadanych zapisany.\n";
	}
	else {
		std::cerr << "B³¹d zapisu pliku.\n";
	}*/

	return 0;
}
