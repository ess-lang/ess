%token LBRACE
%token RBRACE
%token VARDEC
%token LPAREN
%token RPAREN
%token NEWLINE
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
  | IDENTIFIER style_value
    { Ast.StyleExpression(Ast.Style(Ast.ColorProperty, $2)) }
  | IDENTIFIER PROP LBRACE pattern_declaration RBRACE
    { Ast.MatchValueExpression(Ast.ColorProperty, [Ast.Argument($2)], [$4]) }
;

pattern_declaration:
  | IDENTIFIER ARROW style_value
    { Ast.MatchValueClause([Ast.StringPattern([$1])], $3) }
  | IDENTIFIER ARROW style_value
    { Ast.MatchValueClause([Ast.StringPattern([$1])], $3) }
;

%%
