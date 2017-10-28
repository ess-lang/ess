{
  open Parser
  let get = Lexing.lexeme
}

let digit = ['0'-'9']

let hex = ['A'-'F' 'a'-'f' '0'-'9']

rule token = parse
  | [' ' '\t'] { token lexbuf }
  | '@' ['a'-'z']+ { PROP (get lexbuf)}
  | ['a'-'z']+    { IDENTIFIER (get lexbuf) }
  | ['0'-'9']+ "px" { PIXEL }
  | '#' hex hex hex { COLOR_SHORTHEX }
  | '#' hex hex hex hex hex hex { COLOR_HEX }
  | '=' '>' { ARROW }
  | '{' { LBRACE }
  | '}'  { RBRACE }
  | '(' { LPAREN }
  | ')' { RPAREN }
  | _  { token lexbuf }
  | "VARDEC" { VARDEC }
  | eof      { EOF }
