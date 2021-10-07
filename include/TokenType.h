#pragma once

namespace minidb {
enum class TokenType {
    CREATE,
    TABLE,
    INSERT,
    INTO,
    DELETE,
    DROP,
    SELECT,
    FROM,
    WHERE,
    AND,
    OR,
    NOT,
    IDENTIFIER,       // identifier
    COMMA,            // ,
    STAR,             // *
    LBRACKET,         // (
    RBRACKET,         // )
    EQUAL,            // =
    NOT_EQUAL,        // '!=' <>
    LESS,             // <
    LESS_OR_EQUAL,    // <=
    GREATER,          // >
    GREATER_OR_EQUAL, // >=
    PLUS,             // +
    MINUS,            // -
    INTEGER,          // integer number
    FLOAT,            // float number
    STRING,           // string.
    SEMICOLON,        // ;
    ILLEGAL,          // illegal token
    END,              // end of SQL
};
}