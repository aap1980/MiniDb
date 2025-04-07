#include <iostream>
#include <filesystem>
#include "src/Config/Config.h"
#include "src/Database/Database.h"
#include "src/Table/Table.h"
#include "src/Table/QueryCondition.h"
#include "src/Utils/DateTimeUtils.h"

int main() {
	std::filesystem::path current_path = std::filesystem::current_path();

	MiniDb::Config::Config::getInstance().loadConfig("config.ini");

	MiniDb::Database::Database database;

	database.loadAllTables();

	std::vector<MiniDb::Table::Column> columns;
	columns.push_back({ "id", "int" });
	columns.push_back({ "login", "string" });
	columns.push_back({ "name", "string" });
	columns.push_back({ "created_at", "date" });
	database.createTable("Users", columns);

	MiniDb::Table::Table& usersTable = database.getTable("Users");

	usersTable.addRow({ "1", "user1", "", MiniDb::Utils::DateTimeUtils::getCurrentDatetimeAsString() });
	usersTable.addRow({ "2", "user2", "", MiniDb::Utils::DateTimeUtils::getCurrentDatetimeAsString() });
	usersTable.addRow({ "3", "user3", "", MiniDb::Utils::DateTimeUtils::getCurrentDatetimeAsString() });
	usersTable.printTable();
	std::cout << "\n";

	MiniDb::Table::QueryCondition updateCondition;
	updateCondition.addCondition("id", "2");
	std::vector<std::string> updateRow = { "2", "user2", "John Doe", MiniDb::Utils::DateTimeUtils::getCurrentDatetimeAsString() };
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
