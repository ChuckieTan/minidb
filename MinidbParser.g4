sql_stmt: 
      create_table_stmt
    | delete_stmt
    | drop_stmt
    | insert_stmt
    | update_stmt
    | select_stmt
;

list[item]:
    item (COMMA item)*
;

table_name:
    IDENTIFIER
;

create_table_stmt:
    CREATE_ TABLE_ table_name
    OPEN_PAR list[column_def] CLOSE_PAR
;

column_name:
    IDENTIFIER
;

type_name:
    IDENTIFIER (
        OPEN_PAR signed_number CLOSE_PAR
    )?
;

signed_number: (PLUS | MINUS)? INTEGER
;

column_def:
    column_name type_name
;

delete_stmt:
    DELETE_ FROM_ qualified_table_name where_expr?
;

qualified_table_name:
    table_name (AS_ table_alias)?
;

where_expr:
    WHERE_ expr
;

column_name_with_table:
    (table_name DOT)? column_name
;

expr_lvalue:
      literal_value
    | column_name_with_table
;
expr_rvalue:
      literal_value
    | column_name_with_table
;
expr:
      expr_lvalue binary_operator expr_rvalue
        (logical_operator expr)*
    | expr_lvalue (NOT_? IN_) OPEN_PAR select_stmt CLOSE_PAR
    | (NOT_? EXISTS_) OPEN_PAR select_stmt CLOSE_PAR
;

compare_operator:
      ( LT | LT_EQ | GT | GT_EQ )
    | ( ASSIGN | EQ | NOT_EQ )
;

logical_operator:
    AND_ || OR_
;

function:
      function_name OPEN_PAR (table_name | STAR) CLOSE_PAR
;


literal_value:
      numeric_literal
    | STRING_LITERAL
;

drop_table_stmt:
    DROP_ TABLE_ table_name
;

insert_stmt:
    INSERT_ INTO_ table_name
    VALUES_ OPEN_PAR list[literal_value] CLOSE_PAR
;

result_column:
    | table_name DOT STAR
    | column_name (AS_ column_alias)?
;

select_stmt:
    SELECT_ (STAR | list[result_column])
    FROM_ list[table_or_subquery]
    (where_expr)?
;

table_or_subquery:
      qualified_table_name
    | OPEN_PAR select_stmt CLOSE_PAR (AS_ table_alias)?
;

update_stmt:
    UPDATE_ qualified_table_name
    SET_ list[column_name ASSIGN literal_value]
    (where_expr)?
;

any_name:
    IDENTIFIER
    | keyword
    | STRING_LITERAL
    | OPEN_PAR any_name CLOSE_PAR
;

numeric_literal:
      MINUS? INTEGER
    | MINUS? FLOAT
;

keyword:
      COMMA          // ,
    | CREATE_        // create
    | TABLE_         // table
    | IF_            // if
    | NOT_           // not
    | EXISTS_        // exists
    | OPEN_PAR       // (
    | CLOSE_PAR      // )
    | DELETE_        // delete
    | FROM_          // from
    | AS_            // as
    | WHERE_         // where
    | DOT            // .
    | IS_            // is
    | NULL_          // null
    | BETWEEN_       // between
    | AND_           // and
    | IN_            // in
    | DISTINCT_      // distinct
    | STAR           // *
    | DIV            // /
    | MOD            // %
    | PLUS           // +
    | MINUS          // -
    | LT             // <
    | LT_EQ          // <=
    | GT             // >
    | GT_EQ          // >=
    | ASSIGN         // =
    | EQ             // ==
    | NOT_EQ         // != <>
    | AND_           // and
    | OR_            // or
    | STRING_LITERAL // "str"
    | TRUE_          // true
    | FALSE_         // false
    | IDENTIFIER     // identifier
    | DROP_          // drop
    | INSERT_        // insert
    | INTO_          // into
    | ALL_           // all
    | INTEGER        // integer number
    | FLOAT          // float number
;

// expr's grammer with left recursion
// expr:
//       literal_value
//     | function
//     | column_name_with_table
//     | OPEN_PAR expr CLOSE_PAR
//     | unary_operator expr
//     | expr binary_operator expr
//     | expr NOT_? BETWEEN_ expr AND_ expr
//     | expr NOT_? IN_ OPEN_PAR select_stmt CLOSE_PAR
//     | (NOT_)? EXISTS_ OPEN_PAR select_stmt CLOSE_PAR
// ;
 
// expr_first_token:
//       function
//     | literal_value
//     | OPEN_PAR expr CLOSE_PAR
//     | unary_operator expr
//     | column_name_with_table
//     | (NOT_)? EXISTS_ OPEN_PAR select_stmt CLOSE_PAR
// ;
// expr_tail:
//       binary_operator expr
//     | NOT_? BETWEEN_ expr AND_ expr
//     | NOT_? IN_ OPEN_  PAR select_stmt CLOSE_PAR
//     | <empty>
// ;
// expr:
//     expr_first_token expr_tail
// ;
// unary_operator:
//       MINUS
//     | PLUS
// ;
// binary_operator:
//       ( STAR | DIV | MOD )
//     | ( PLUS | MINUS )
//     | ( LT | LT_EQ | GT | GT_EQ )
//     | ( ASSIGN | EQ | NOT_EQ | IS_ NOT_? | IN_ )
//     | AND_
//     | OR_
