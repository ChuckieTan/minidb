sql_stmt: 
      create_table_stmt
    | delete_stmt
    | drop_stmt
    | insert_stmt
    | select_stmt
    | update_stmt
;

list[item]:
    item (COMMA item)*
;

table_name:
    IDENTIFIER
;

create_table_stmt:
    CREATE_ TABLE_ (IF_ NOT_ EXISTS_)? table_name
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

expr_first_token:
      function
    | literal_value
    | OPEN_PAR expr CLOSE_PAR
    | unary_operator expr
    | column_name_with_table
    | (NOT_)? EXISTS_ OPEN_PAR select_stmt CLOSE_PAR
;

expr_tail:
      binary_operator expr
    | NOT_? BETWEEN_ expr AND_ expr
    | NOT_? IN_ OPEN_PAR select_stmt CLOSE_PAR
    | <empty>
;

expr:
    expr_first_token expr_tail
;

function:
      function_name OPEN_PAR ((DISTINCT_? table_name) | STAR)? CLOSE_PAR

binary_operator:
      ( STAR | DIV | MOD )
    | ( PLUS | MINUS )
    | ( LT | LT_EQ | GT | GT_EQ )
    | ( ASSIGN | EQ | NOT_EQ | IS_ NOT_? | IN_ )
    | AND_
    | OR_

literal_value:
      numeric_literal
    | STRING_LITERAL
    | NULL_
    | TRUE_
    | FALSE_
;

drop_stmt:
    DROP_ TABLE_ (IF_ EXISTS_)? table_name
;

insert_stmt:
    INSERT_ INTO_ table_name (
        OPEN_PAR list[column_name] CLOSE_PAR
    )? VALUES_ OPEN_PAR list[literal_value] CLOSE_PAR
;

result_column:
    | table_name DOT STAR
    | column_name (AS_ column_alias)?
;

select_stmt:
    SELECT_ (DISTINCT_ | ALL_)? (STAR | list[result_column])
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
      INTEGER
    | FLOAT
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