#pragma once

namespace minidb {
enum class TokenType {
    CHAR,             // char(n)
    INT,              // int(n)
    CREATE_TABLE,     // create table ...
    INSERT_INTO,      // insert into ...
    DELETE_FROM,      // delete from ...
    DROP_TABLE,       // drop table ...
    SELECT,           // select
    FROM,             // from
    WHERE,            // where
    AND,              // and
    OR,               // or
    NOT,              // not
    ID,               // id
    COMMA,            // ,
    LBRACKET,         // (
    RBRACKET,         // )
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
    VARIABLE,         // variable
    ILLEGAL,           // illegal token
    END,              // end of SQL
};
}