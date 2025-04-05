#include "include/MiniDb/Metadata/TableMetadataWriter.h"
#include <iostream>
#include <filesystem>

using namespace MiniDb::Metadata;

int main() {
	std::filesystem::path current_path = std::filesystem::current_path();

	TableMetadataWriter writer("Users");
	writer.addColumn("id", "int");
	writer.addColumn("login", "string");

	std::filesystem::path file_path = current_path / "Users.md";
	if (writer.saveToFile(file_path.string())) {
		std::cout << "Plik metadanych zapisany.\n";
	}
	else {
		std::cerr << "B³¹d zapisu pliku.\n";
	}

	return 0;

	std::cout << "Hello World!\n";
}
