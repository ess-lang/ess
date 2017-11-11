{
  open Parser
  let get = Lexing.lexeme

  let hex_to_int hex_char1 hex_char2 = int_of_string ("0x" ^ (String.make 1 hex_char1) ^ (String.make 1 hex_char2))
  let hex2_to_int hex_char = int_of_string ("0x" ^ (String.make 2 hex_char))

  let int_of_maybe_string str =
    match str with
    | "" -> None
    | _ -> Some (int_of_string str)
}

let digit = ['0'-'9']

let hex = ['A'-'F' 'a'-'f' '0'-'9']

let alpha = ['A'-'Z' '-' 'a'-'z']

rule token = parse
  | '\n' { NEWLINE }
  | '_' { UNDERSCORE }
  | '=' '>' { ARROW }
  | '=' { EQ }
  | ',' { COMMA }
  | '{' { LBRACE }
  | '}'  { RBRACE }
  | '[' { LBRACKET }
  | ']' { RBRACKET }
  | '(' { LPAREN }
  | ')' { RPAREN }
  | '|' { PIPE }
  | "VARDEC" { VARDEC }
  | "/*"  { comment lexbuf; token lexbuf }
  | [' ' '\t'] { token lexbuf }
  | '@' (['a'-'z']+ as id) { PROP (id)}
  | "boolean" { BOOLEAN }
  | "false" { FALSE }
  | "true" { TRUE }
  | '"' (['a'-'z']+ as str) '"' { STRING(str) }
  | alpha+    { IDENTIFIER (get lexbuf) }
  | ['0'-'9']+ "px" { PIXEL }
  | (['0'-'9']+ as start_num)'.' '.' (['0'-'9']* as end_num)
    { RANGE(int_of_string start_num, int_of_maybe_string end_num) }
  | '#' (hex as r) (hex as g) (hex as b)
    { COLOR_SHORTHEX(hex2_to_int r, hex2_to_int g, hex2_to_int b) }
  | '#' (hex as r1) (hex as r2) (hex as g1) (hex as g2) (hex as b1) (hex as b2)
    { COLOR_HEX(hex_to_int r1 r2, hex_to_int g1 g2, hex_to_int b1 b2) }
  | _  { token lexbuf }
  | eof      { EOF }

and comment = parse
    | "/*"          { comment lexbuf; comment lexbuf }
    | "*/"          { () }
    | _             { comment lexbuf }
    | eof           { failwith "lexer: comment not terminated" }
