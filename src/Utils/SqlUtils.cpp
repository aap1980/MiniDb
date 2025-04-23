#include "SqlUtils.h"
#include <stdexcept>

namespace MiniDb::Utils {

	std::string SqlUtils::dataTypeToString(hsql::DataType dataType) {
		switch (dataType) {
		case hsql::DataType::INT: return "INT";
		case hsql::DataType::BIGINT: return "INT";
		case hsql::DataType::VARCHAR: return "VARCHAR";
		case hsql::DataType::CHAR: return "VARCHAR";
		case hsql::DataType::TEXT: return "VARCHAR";
		case hsql::DataType::DOUBLE: return "DOUBLE";
		case hsql::DataType::FLOAT: return "DOUBLE";
		case hsql::DataType::DATE: return "DATE";
		case hsql::DataType::DATETIME: return "DATETIME";
		case hsql::DataType::BOOLEAN: return "BOOLEAN";
		case hsql::DataType::DECIMAL:
			throw std::runtime_error("Unsupported data type: DECIMAL");
		case hsql::DataType::UNKNOWN:
		default:
			throw std::runtime_error("Unsupported or unknown data type.");
		}
	}

}
