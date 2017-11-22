%{

open Utils

%}

%token LBRACE
%token RBRACE
%token VARDEC
%token LPAREN
%token RPAREN
%token LBRACKET
%token RBRACKET
%token COMMA
%token PIPE
%token BOOLEAN
%token TRUE FALSE
%token NEWLINE
%token UNDERSCORE
%token EQ
%token <int * int option>RANGE
%token PIXEL
%token <int * int * int>COLOR_SHORTHEX
%token <int * int * int>COLOR_HEX
%token ARROW
%token <string>IDENTIFIER
%token <string>PROP
%token <string>STRING

%token EOF

%start input
%type <Ast.stylesheet> input

%%

input:
  | NEWLINE* s = program EOF { Ast.Stylesheet s }

program:
  | sl = statement_list? { list_maybe sl }

statement_list:
  | _statement NEWLINE* { [$1] }
  | _statement NEWLINE+ statement_list { $1 :: $3 }

_statement:
  | _class { $1 }
  | VARDEC { Ast.VariableDeclaration("some-var", Ast.Px(20.0)) }

_class:
  | id = IDENTIFIER LBRACKET NEWLINE* cb = class_body RBRACKET
    { Ast.ClassDeclaration(id, cb) }

class_body:
  | body_block? { Ast.ClassBody(list_maybe($1))}

body_block:
  | body_expression NEWLINE* { [$1] }
  | body_expression NEWLINE+ body_block { $1 :: $3 }

prop_type:
  | p = PROP EQ ptv = prop_type_value
    { Ast.ClassPropDeclaration(Ast.Argument(p), ptv) }

prop_type_value:
  | ls = separated_nonempty_list(PIPE, STRING)
    { Ast.StringEnumType(ls) }
  | BOOLEAN
    { Ast.BooleanType }

style_value:
  | COLOR_HEX { Ast.ColorRGB($1) }
  | COLOR_SHORTHEX { Ast.ColorRGB($1) }
  | IDENTIFIER { Ast.StringLiteral($1) }

body_expression:
  | prop_type { $1 }
  | base_style_thing { Ast.StyleExpression($1) }
  | prop_arguments LBRACE NEWLINE* RBRACE
    { Ast.MatchBlockExpression($1, []) }
  | prop_arguments LBRACE NEWLINE+ match_block_body RBRACE
    { Ast.MatchBlockExpression($1, $4) }
  | IDENTIFIER prop_arguments LBRACE NEWLINE* RBRACE
    { Ast.MatchValueExpression(string_to_property $1, $2, []) }
  | IDENTIFIER prop_arguments LBRACE NEWLINE+ match_value_body RBRACE
    { Ast.MatchValueExpression(string_to_property $1, $2, $5) }

prop_arguments:
  | prop_val { [$1] }
  | delimited(LPAREN, separated_nonempty_list(COMMA, prop_val), RPAREN) { $1 }

prop_val:
  | PROP { Ast.Argument($1) }

base_style_thing:
  | IDENTIFIER style_value
    { Ast.Style(string_to_property $1, $2) }

match_block_body:
  | match_block_clause NEWLINE* { [$1] }
  | match_block_clause NEWLINE+ match_block_body { $1 :: $3 }

match_value_body:
  | match_value_clause NEWLINE* { [$1] }
  | match_value_clause NEWLINE+ match_value_body { $1 :: $3 }

match_value_clause:
  | pattern_def ARROW style_value
    { Ast.MatchValueClause($1, $3) }

match_block_clause:
  | pattern_def ARROW LBRACKET NEWLINE* RBRACKET
    { Ast.MatchBlockClause($1, []) }
  | pattern_def ARROW LBRACKET NEWLINE* match_block_clause_body RBRACKET
    { Ast.MatchBlockClause($1, $5) }
  | pattern_def ARROW base_style_thing
    { Ast.MatchBlockClause($1, [$3]) }

pattern_def:
  | pattern { [$1] }
  | p = delimited(LPAREN, separated_nonempty_list(COMMA, pattern), RPAREN) { p }

pattern:
  | ls = separated_nonempty_list(PIPE, STRING)
    { Ast.StringPattern(ls) }
  | TRUE
    { Ast.BooleanPattern(true) }
  | FALSE
    { Ast.BooleanPattern(false) }
  | RANGE
    { Ast.NumberRangePattern($1) }
  | UNDERSCORE
    { Ast.FallthroughPattern }

match_block_clause_body:
  | base_style_thing NEWLINE* { [$1] }
  | base_style_thing NEWLINE+ match_block_clause_body { $1 :: $3 }

%%
