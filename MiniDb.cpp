#include <iostream>
#include <filesystem>
#include "src/Config/Config.h"
#include "src/Database/Database.h"
#include "src/Metadata/TableMetadataWriter.h"
#include "src/Table/Table.h"
#include "src/Table/QueryCondition.h"

int main() {
	std::filesystem::path current_path = std::filesystem::current_path();

	MiniDb::Config::Config::getInstance().loadConfig("config.ini");

	MiniDb::Database::Database database;

	database.loadAllTables();

	MiniDb::Table::Table& usersTable = database.getTable("Users");

	usersTable.addRow({ "1", "user1" });
	usersTable.addRow({ "2", "user2" });
	usersTable.addRow({ "3", "user3" });
	usersTable.printTable();
	std::cout << "\n";

	MiniDb::Table::QueryCondition updateCondition;
	updateCondition.addCondition("id", "2");
	std::vector<std::string> updateRow = { "2", "John Doe" };
	usersTable.updateRow(updateCondition, updateRow);
	usersTable.printTable();
	std::cout << "\n";

	MiniDb::Table::QueryCondition deleteCondition;
	deleteCondition.addCondition("id", "1");
	usersTable.deleteRow(deleteCondition);
	usersTable.printTable();
	std::cout << "\n";

	return 0;
}
