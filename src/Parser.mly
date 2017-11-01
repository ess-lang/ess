%token LBRACE
%token RBRACE
%token VARDEC
%token LPAREN
%token RPAREN
%token PIXEL
%token COLOR_SHORTHEX
%token COLOR_HEX
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
  | _statement { [$1] }
  | _statement program { $1 :: $2 }
;

_statement:
  | _class { $1 }
  | VARDEC { Ast.VariableDeclaration("some-var", Ast.Px(20.0)) }
;

_class:
  | IDENTIFIER LBRACE class_body RBRACE
    { Ast.ClassDeclaration($1, $3) }
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
  | prop_types style_block { Ast.ClassBody($1, $2) }
;

style_block:
  | style_expression { [$1] }
  | style_expression style_block { $1 :: $2 }
;

style_expression:
  | IDENTIFIER IDENTIFIER
    { Ast.StyleExpression(Ast.Style(Ast.ColorProperty, Ast.StringLiteral($2))) }
  | IDENTIFIER PROP LBRACE pattern_declaration RBRACE
    { Ast.MatchValueExpression(Ast.ColorProperty, [Ast.Argument($2)], [$4]) }
;

pattern_declaration:
  | IDENTIFIER ARROW IDENTIFIER
    { Ast.MatchValueClause([Ast.StringPattern([$1])], Ast.StringLiteral($3)) }
;

%%
