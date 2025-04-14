#include <iostream>
#include "Console.h"
#include "../Executors/SelectExecutor.h"
#include "../SqlParser/SQLParser.h"
#include "../SqlParser/SQLParserResult.h"
#include "../SqlParser/util/sqlhelper.h"

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
		hsql::SQLParserResult result;
		hsql::SQLParser::parse(command, &result);

		//for (const auto* stmt : result.getStatements()) {
		//	handleStatement(stmt);
		//}

		if (result.isValid()) {
			printf("Parsed successfully!\n");
			printf("Number of statements: %lu\n", result.size());

			for (auto i = 0u; i < result.size(); ++i) {
				// Print a statement summary.
				hsql::printStatementInfo(result.getStatement(i));
			}
		}
		else {
			fprintf(stderr, "Given string is not a valid SQL query.\n");
			fprintf(stderr, "%s (L%d:%d)\n",
				result.errorMsg(),
				result.errorLine(),
				result.errorColumn());
		}

	}

}
