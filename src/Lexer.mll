{
  open Parser
  let get = Lexing.lexeme

  let hex_to_int hex_char1 hex_char2 = int_of_string ("0x" ^ (String.make 1 hex_char1) ^ (String.make 1 hex_char2))
  let hex2_to_int hex_char = int_of_string ("0x" ^ (String.make 2 hex_char))
}

let digit = ['0'-'9']

let hex = ['A'-'F' 'a'-'f' '0'-'9']

rule token = parse
  | '\n' { NEWLINE }
  | [' ' '\t'] { token lexbuf }
  | '@' ['a'-'z']+ { PROP (get lexbuf)}
  | ['a'-'z']+    { IDENTIFIER (get lexbuf) }
  | ['0'-'9']+ "px" { PIXEL }
  | ['0'-'9']+ '.' '.' ['0'-'9']+ { RANGE }
  | '#' (hex as r) (hex as g) (hex as b)
    { COLOR_SHORTHEX(hex2_to_int r, hex2_to_int g, hex2_to_int b) }
  | '#' (hex as r1) (hex as r2) (hex as g1) (hex as g2) (hex as b1) (hex as b2)
    { COLOR_HEX(hex_to_int r1 r2, hex_to_int g1 g2, hex_to_int b1 b2) }
  | '=' '>' { ARROW }
  | '{' { LBRACE }
  | '}'  { RBRACE }
  | '[' { LBRACKET }
  | ']' { RBRACKET }
  | '(' { LPAREN }
  | ')' { RPAREN }
  | _  { token lexbuf }
  | "VARDEC" { VARDEC }
  | eof      { EOF }
