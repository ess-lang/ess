%token LBRACE
%token RBRACE
%token VARDEC
%token LPAREN
%token RPAREN
%token LBRACKET
%token RBRACKET
%token COMMA
%token COLON
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
%token <string>ELEMENT_ID
%token <string>PROP
%token <string>STRING

%token EOF

%start input
%type <Ast.stylesheet> input

%%

input:
  | NEWLINE* s = program EOF { Ast.Stylesheet s }

program:
  | sl = statement_list? { Utils.list_maybe sl }

statement_list:
  | s = _statement NEWLINE* { [s] }
  | s = _statement NEWLINE+ sl = statement_list { s :: sl }

_statement:
  | e = element { e }

element:
  | id = ELEMENT_ID ep = element_params_specifier? b = block
    { Ast.ElementDeclaration(($startpos, $endpos), id, Utils.list_maybe(ep), b) }

element_params_specifier:
  | LPAREN NEWLINE* RPAREN
    { [] }
  | LPAREN e = element_params RPAREN
    { e }
  | LPAREN NEWLINE+ e = element_params RPAREN
    { e }

element_params:
  | f = element_param { [f] }
  | f = element_param c = element_params { f :: c }

element_param:
  | r = element_param_entry NEWLINE* { r }
  | r = element_param_entry COMMA NEWLINE* { r }
  | r = element_param_entry NEWLINE+ COMMA NEWLINE* { r }

element_param_entry:
  | p = parameter NEWLINE* COLON NEWLINE* parameter_typedef
    { p }

block:
  | LBRACKET NEWLINE* br = block_rules? RBRACKET
    { Ast.Block(($startpos, $endpos), Utils.list_maybe(br)) }

block_rules:
  | e = block_entry { [e] }
  | e = block_entry b = block_rules { e :: b }

block_entry:
  | r = rule NEWLINE* { r }
  | r = rule COMMA NEWLINE* { r }
  | r = rule NEWLINE+ COMMA NEWLINE* { r }

rule:
  | r = attribute_rule { r }
  | r = composition_rule { r }
  | r = match_rule { r }

attribute_rule:
  | attr = IDENTIFIER exp = expression { Ast.AttributeRule(($startpos, $endpos), attr, exp) }

composition_rule:
  | id = ELEMENT_ID { Ast.CompositionRule(($startpos, $endpos), id) }

match_rule:
  | parameter_list LBRACE NEWLINE* RBRACE
    { Ast.MatchRule(($startpos, $endpos), $1, []) }
  | parameter_list LBRACE NEWLINE+ match_rule_body RBRACE
    { Ast.MatchRule(($startpos, $endpos), $1, $4) }

match_rule_body:
  | match_rule_clause NEWLINE* { [$1] }
  | match_rule_clause NEWLINE+ match_rule_body { $1 :: $3 }

match_rule_clause:
  | p = pattern_def ARROW b = block COMMA?
    { Ast.MatchRuleClause(($startpos, $endpos), p, b) }

expression:
  | e = plain_expression { e }
  | e = match_expression { e }
  | e = record_expression { e }
  | LPAREN e = expression RPAREN { e }

plain_expression:
  | IDENTIFIER { Ast.LiteralExpression(($startpos, $endpos)) }

record_expression:
  | LPAREN e = record_entries? RPAREN
    { Ast.RecordExpression(($startpos, $endpos), Utils.list_maybe(e)) }
  | LPAREN NEWLINE+ e = record_entries RPAREN
    { Ast.RecordExpression(($startpos, $endpos), e) }
  | LPAREN NEWLINE+ RPAREN
    { Ast.RecordExpression(($startpos, $endpos), []) }

record_entries:
  | f = record_entry { [f] }
  | f = record_entry c = record_entries { f :: c }

record_entry:
  | r = record_field NEWLINE* { r }
  | r = record_field COMMA NEWLINE* { r }
  | r = record_field NEWLINE+ COMMA NEWLINE* { r }

record_field:
  | f = IDENTIFIER NEWLINE* COLON NEWLINE* v = IDENTIFIER
    { Ast.RecordField(($startpos, $endpos), f, Ast.TextLiteral($startpos, $endpos))}

match_expression:
  | parameter_list LBRACE NEWLINE* RBRACE
    { Ast.MatchExpression(($startpos, $endpos), $1, []) }
  | parameter_list LBRACE NEWLINE+ match_expression_body RBRACE
    { Ast.MatchExpression(($startpos, $endpos), $1, $4) }

match_expression_body:
  | match_expression_clause NEWLINE* { [$1] }
  | match_expression_clause NEWLINE+ match_expression_body { $1 :: $3 }

match_expression_clause:
  | pattern_def ARROW expression
    { Ast.MatchExpressionClause(($startpos, $endpos), $1, $3) }

parameter_list:
  | parameter { [$1] }
  | delimited(LPAREN, separated_nonempty_list(COMMA, parameter), RPAREN) { $1 }

parameter:
  | p = PROP { Ast.Parameter(($startpos, $endpos), p) }

pattern_def:
  | p = pattern { [p] }
  | p = delimited(LPAREN, separated_nonempty_list(COMMA, pattern), RPAREN) { p }

pattern:
  | ls = separated_nonempty_list(PIPE, STRING)
    { Ast.StringPattern(($startpos, $endpos), ls) }
  | TRUE
    { Ast.BooleanPattern(($startpos, $endpos), true) }
  | FALSE
    { Ast.BooleanPattern(($startpos, $endpos), false) }
  | RANGE
    { Ast.NumberRangePattern(($startpos, $endpos), $1) }
  | UNDERSCORE
    { Ast.FallthroughPattern(($startpos, $endpos)) }

parameter_typedef:
  | ls = separated_nonempty_list(PIPE, STRING)
    { Ast.StringPattern(($startpos, $endpos), ls) }
  | BOOLEAN
    { Ast.FallthroughPattern(($startpos, $endpos)) }

%%
