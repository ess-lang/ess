let lexbuf =
  Lexing.from_string " VARDEC VARDEC zz { @haha woo color red background green }";

Parser.input Lexer.token lexbuf;
