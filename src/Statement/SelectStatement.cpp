#include "SelectStatement.h"
#include <iostream>
#include "../Database/Database.h"
#include "../SqlParser/SQLParser.h"

namespace MiniDb::Statement {

	SelectStatement::SelectStatement(const hsql::SelectStatement* statement) {
		if (statement->fromTable != nullptr)
			_tableName = statement->fromTable->getName();

		_selectAll = statement->selectList->size() == 1 &&
			(*statement->selectList)[0]->type == hsql::kExprStar;
	}

	void SelectStatement::execute(MiniDb::Database::Database& database) const {
		if (!_selectAll) {
			std::cout << "Obs³uga innych kolumn ni¿ * niezaimplementowana.\n";
			return;
		}

		MiniDb::Table::Table& table = database.getTable(_tableName);
		table.loadDataFromFile();
		table.printTable();

		//const auto& columns = table.columns.getColumns();
		//const auto& rows = table.rows;

		//for (const auto& col : columns)
		//	std::cout << col.name << "\t";
		//std::cout << "\n";

		//for (const auto& row : rows) {
		//	for (const auto& val : row)
		//		std::cout << val << "\t";
		//	std::cout << "\n";
		//}
	}

}
