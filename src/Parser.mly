%token LBRACE
%token RBRACE
%token VARDEC
%token LPAREN
%token RPAREN
%token LBRACKET
%token RBRACKET
%token NEWLINE
%token RANGE
%token PIXEL
%token <int * int * int>COLOR_SHORTHEX
%token <int * int * int>COLOR_HEX
%token ARROW
%token <string>IDENTIFIER
%token <string>PROP

%token EOF

%start input
%type <unit> input

%%

input:
  | EOF { Js.log "nothing"}
  | program EOF { Js.log @@ Js.Json.stringifyAny $1 }
;

program:
  | _statement NEWLINE* { [$1] }
  | _statement NEWLINE+ program { $1 :: $3 }
;

_statement:
  | _class { $1 }
  | VARDEC { Ast.VariableDeclaration("some-var", Ast.Px(20.0)) }
;

_class:
  | IDENTIFIER LBRACE NEWLINE* class_body RBRACE
    { Ast.ClassDeclaration($1, $4) }
  | IDENTIFIER LBRACE NEWLINE* RBRACE
    { Ast.ClassDeclaration($1, Ast.ClassBody([], [])) }
;

prop_types:
  | prop_type { [$1] }
  | prop_type prop_types { $1 :: $2}
;

prop_type:
  | PROP IDENTIFIER { Ast.ClassPropDeclaration($1, $2) }
;

class_body:
  | prop_types { Ast.ClassBody($1, []) }
  | style_block { Ast.ClassBody([], $1)}
  | prop_types NEWLINE+ style_block { Ast.ClassBody($1, $3) }
;

style_block:
  | style_expression NEWLINE* { [$1] }
  | style_expression NEWLINE+ style_block { $1 :: $3 }
;

style_value:
  | COLOR_HEX { Ast.ColorRGB($1) }
  | COLOR_SHORTHEX { Ast.ColorRGB($1) }
  | IDENTIFIER { Ast.StringLiteral($1) }
;

style_expression:
  | base_style_thing { Ast.StyleExpression($1) }
  | PROP LBRACE NEWLINE* RBRACE
    { Ast.MatchBlockExpression([Ast.Argument($1)], []) }
  | PROP LBRACE NEWLINE+ match_block_body RBRACE
    { Ast.MatchBlockExpression([Ast.Argument($1)], $4) }
  | IDENTIFIER PROP LBRACE NEWLINE* RBRACE
    { Ast.MatchValueExpression(Ast.ColorProperty, [Ast.Argument($2)], []) }
  | IDENTIFIER PROP LBRACE NEWLINE+ match_value_body RBRACE
    { Ast.MatchValueExpression(Ast.ColorProperty, [Ast.Argument($2)], $5) }
;

base_style_thing:
  | IDENTIFIER style_value
    { Ast.Style(Ast.ColorProperty, $2) }
;

match_block_body:
  | match_block_clause NEWLINE* { [$1] }
  | match_block_clause NEWLINE+ match_block_body { $1 :: $3 }
;

match_value_body:
  | match_value_clause NEWLINE* { [$1] }
  | match_value_clause NEWLINE+ match_value_body { $1 :: $3 }
;

match_value_clause:
  | pattern ARROW style_value
    { Ast.MatchValueClause($1, $3) }
;

match_block_clause:
  | pattern ARROW LBRACKET NEWLINE* RBRACKET
    { Ast.MatchBlockClause($1, []) }
  | pattern ARROW LBRACKET NEWLINE* match_block_clause_body RBRACKET
    { Ast.MatchBlockClause($1, $5) }
  | pattern ARROW base_style_thing
    { Ast.MatchBlockClause($1, [$3]) }
;

pattern:
  | IDENTIFIER { [Ast.StringPattern([$1])] }
  | RANGE { [Ast.NumberRangePattern(5, 25)] }
;

match_block_clause_body:
  | base_style_thing NEWLINE* { [$1] }
  | base_style_thing NEWLINE+ match_block_clause_body { $1 :: $3 }
;

%%
