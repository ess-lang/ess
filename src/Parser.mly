%{

%}

%token LBRACE RBRACE EOF VARDEC LPAREN RPAREN PIXEL COLOR_SHORTHEX COLOR_HEX ARROW
%token <string>IDENTIFIER
%token <string>PROP

%start input
%type <unit> input

%% /* Grammar rules and actions follow */

input: EOF { Js.log "nothing"}
     | program EOF { Js.log @@ Js.Json.stringifyAny $1 }
    ;

program: _statement { [$1] }
      | _statement program { $1 :: $2 }
      ;

_statement: _class { $1 }
   | VARDEC { Ast.VariableDeclaration }
   ;

_class: IDENTIFIER LBRACE classbody RBRACE { Ast.ClassDeclaration($1, $3) }
  ;

proptypes: proptype { [$1] }
            | proptype proptypes { $1 :: $2}
  ;

proptype: PROP IDENTIFIER { Ast.Proptype($1, $2) }
        ;

classbody: proptypes { Ast.ClassBody($1, []) }
        | declist { Ast.ClassBody([], $1)}
        | proptypes declist { Ast.ClassBody($1, $2) }


declist: decl { [$1] }
       | decl declist { $1 :: $2 }
       ;

decl: IDENTIFIER declvalue { Ast.Declaration($1, $2) }
    ;

declvalue: IDENTIFIER { Ast.ValueLiteral($1)}
  | PROP LBRACE patterndecl RBRACE { Ast.MatchValue($1, [$3])}
  ;

patterndecl : IDENTIFIER ARROW IDENTIFIER { Ast.PatternDeclaration($1, $3) }


%%
