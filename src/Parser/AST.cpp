#include "AST.h"
#include <iostream>
#include <iomanip>

namespace MiniDb::Parser {

	void printIndent(std::ostream& os, int indent) {
		os << std::setw(indent * 2) << ""; // 2 spacje na poziom wciêcia
	}

	std::string operatorToString(BinaryOperatorType op) {
		switch (op) {
		case BinaryOperatorType::EQUALS: return "=";
		case BinaryOperatorType::AND:    return "AND";
		default:                         return "UNKNOWN_OP";
		}
	}


	void LiteralExpression::print(std::ostream& os, int indent) const {
		printIndent(os, indent);
		os << "Literal: ";
		// U¿yj std::visit do obs³ugi ró¿nych typów w wariancie
		std::visit([&os](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, int>)
				os << arg;
			else if constexpr (std::is_same_v<T, std::string>)
				os << "'" << arg << "'"; // Dodaj apostrofy dla stringów
			}, value);
		os << std::endl;
	}

	void ColumnIdentifierExpression::print(std::ostream& os, int indent) const {
		printIndent(os, indent);
		os << "Column: " << columnName << std::endl;
	}

	void BinaryOperationExpression::print(std::ostream& os, int indent) const {
		printIndent(os, indent);
		os << "BinaryOp: " << operatorToString(op) << std::endl;
		// Rekurencyjnie drukuj lewe i prawe poddrzewo z wiêkszym wciêciem
		if (left) {
			printIndent(os, indent);
			os << " Left:" << std::endl;
			left->print(os, indent + 1);
		}
		if (right) {
			printIndent(os, indent);
			os << " Right:" << std::endl;
			right->print(os, indent + 1);
		}
	}

	void SelectStatement::print() const {
		std::cout << "SELECT ";
		for (size_t i = 0; i < columns.size(); ++i) {
			std::cout << columns[i] << (i == columns.size() - 1 ? "" : ", ");
		}
		std::cout << "\nFROM " << fromTable;
		if (whereCondition) {
			std::cout << "\nWHERE:" << std::endl;
			// Wywo³aj print na korzeniu drzewa wyra¿eñ WHERE
			whereCondition.value()->print(std::cout, 1);
		}
		else {
			std::cout << "\n(No WHERE clause)";
		}
		std::cout << ";" << std::endl;
	}

}
