#pragma once
#include <string>
#include <ostream>

namespace MiniDb::Parser {

    // Typy tokenów
    enum class TokenType {
        SELECT_KEYWORD,
        FROM_KEYWORD,
        WHERE_KEYWORD,
        IDENTIFIER,       // np. nazwa kolumny, nazwa tabeli
        INTEGER_LITERAL,  // np. 1, 123
        // STRING_LITERAL, // Do dodania w przyszłości
        COMMA,            // ,
        EQUAL_OPERATOR,   // =
        SEMICOLON,        // ;
        END_OF_FILE,      // Koniec wejścia
        UNKNOWN           // Nierozpoznany token
    };

    // Funkcja pomocnicza do wypisywania typu tokenu
    std::string tokenTypeToString(TokenType type);

    // Struktura reprezentująca pojedynczy token
    struct Token {
        TokenType type;
        std::string value; // Rzeczywista wartość tokenu (np. "SELECT", "id", "1")

        Token(TokenType t = TokenType::UNKNOWN, std::string v = "") : type(t), value(std::move(v)) {}

        friend std::ostream& operator<<(std::ostream& os, const Token& token);
    };

}