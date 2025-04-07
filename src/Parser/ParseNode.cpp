#include "ParseNode.h"

namespace MiniDb::Parser {

	ParseNode::ParseNode(NodeType type, const std::string& value)
		: type(type), value(value) {
	}

	void ParseNode::addChild(const ParseNode& child) {
		children.push_back(child);
	}

}