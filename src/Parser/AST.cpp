#include "AST.h"
#include <iostream>

namespace MiniDb::Parser {

	void SelectStatement::print() const {
		std::cout << "SELECT ";
		for (size_t i = 0; i < columns.size(); ++i) {
			std::cout << columns[i] << (i == columns.size() - 1 ? "" : ", ");
		}
		std::cout << "\nFROM " << fromTable;
		if (whereCondition) {
			std::cout << "\nWHERE " << whereCondition->columnName << " "
				<< whereCondition->op << " " << whereCondition->literalValue;
		}
		std::cout << ";" << std::endl;
	}

}
