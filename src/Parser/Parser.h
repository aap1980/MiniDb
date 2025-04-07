#pragma once
#include <string>
#include "ParseNode.h"

namespace MiniDb::Parser {

    class Parser {
    public:
        Parser(const std::string& query);
        ParseNode parse();

    private:
        std::string query;
        size_t currentIndex;

        ParseNode parseSelect();
        ParseNode parseColumn();
        void skipWhitespace();
    };

}