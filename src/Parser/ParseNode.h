#pragma once
#include <vector>
#include <string>

namespace MiniDb::Parser {

    class ParseNode {
    public:
        enum class NodeType {
            SELECT,
            FROM,
            WHERE,
            COLUMN,
            VALUE,
            OPERATOR
        };

        NodeType type;
        std::string value;
        std::vector<ParseNode> children;

        ParseNode(NodeType type, const std::string& value = "");
        void addChild(const ParseNode& child);
    };

}