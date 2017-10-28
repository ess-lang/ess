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
  | VARDEC { Ast.VariableDeclaration }
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
  | PROP IDENTIFIER { Ast.Proptype($1, $2) }
;

class_body:
  | prop_types { Ast.ClassBody($1, []) }
  | style_list { Ast.ClassBody([], $1)}
  | prop_types style_list { Ast.ClassBody($1, $2) }
;

style_list:
  | style { [$1] }
  | style style_list { $1 :: $2 }
;

style:
  | IDENTIFIER style_value
    { Ast.Style($1, $2) }
;

style_value:
  | IDENTIFIER
    { Ast.ValueLiteral($1)}
  | PROP LBRACE pattern_declaration RBRACE
    { Ast.MatchValue($1, [$3])}
;

pattern_declaration:
  | IDENTIFIER ARROW IDENTIFIER
    { Ast.PatternDeclaration($1, $3) }
;

%%
