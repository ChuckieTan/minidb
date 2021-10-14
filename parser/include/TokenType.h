#pragma once

namespace minidb::parser {
enum class TokenType {
    CREATE,
    TABLE,
    INSERT,
    INTO,
    DELETE,
    DROP,
    SELECT,
    FROM,
    AS,
    WHERE,
    AND,
    OR,
    NOT,
    IDENTIFIER,       // identifier
    COMMA,            // ,
    STAR,             // *
    LBRACKET,         // (
    RBRACKET,         // )
    IN,               // in
    IF,               // if
    EXISTS,           // exists
    IS,               // is
    NULL_,            // null
    TRUE,             // true
    FALSE,            // false
    EQUAL,            // ==
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

    DOT,              // .
    BETWEEN,          // between
    DISTINCT,         // distinct
    DIV,              // /
    MOD,              // %
    ASSIGN,           // =
    ALL,              // all
};
}