#include <iostream>
#include "Console.h"
#include "../Parser/Token.h"
#include "../Parser/Lexer.h"
#include "../Parser/AST.h"
#include "../Parser/Parser.h"
#include "../Executors/SelectExecutor.h"

namespace MiniDb::Console {

	void Console::run() {
		std::string input;

		std::cout << "MiniDb Console - Enter commands (type 'exit' to quit):\n";

		while (true) {
			std::cout << "> ";
			std::getline(std::cin, input);

			if (input == "exit") {
				break;
			}

			parseCommand(input);
		}
	}

	void Console::parseCommand(const std::string& command) {
		try {
			MiniDb::Parser::Lexer lexer(command);
			MiniDb::Parser::Parser parser(lexer);
			MiniDb::Parser::SelectStatement ast = parser.parseSelectStatement();
			std::cout << "Parsed Statement (AST):" << std::endl;
			ast.print();
			//MiniDb::Executor::SelectExecutor selectExecutor(MiniDb::Database::Database::getInstance());
		}
		catch (const std::exception& e) {
			std::cout << "Error: " << e.what() << "\n";
		}
	}

}
