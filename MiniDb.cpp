#include <iostream>
#include <filesystem>
#include "src/Config/Config.h"
#include "src/Metadata/TableMetadataWriter.h"
#include "src/Table/Table.h"

using namespace MiniDb::Table;

int main() {
	std::filesystem::path current_path = std::filesystem::current_path();

	MiniDb::Config::Config config;

	if (!config.loadConfig("config.ini")) {
		std::cerr << "B³¹d wczytywania pliku konfiguracyjnego." << std::endl;
		return 1;
	}


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

	Table usersTable("Users");

	// £adujemy metadane tabeli
	if (usersTable.loadMetadata("Users.md")) {
		std::cout << "Tabela metadanych za³adowana.\n";

		// Dodajemy wiersze do tabeli
		usersTable.addRow({ "1", "user1" });
		usersTable.addRow({ "2", "user2" });

		// Wyœwietlamy tabelê
		usersTable.printTable();
	}
	else {
		std::cerr << "B³¹d ³adowania pliku metadanych.\n";
	}

	return 0;

	std::cout << "Hello World!\n";
}
