let lexbuf =
  Lexing.from_string(
    " VARDEC VARDEC zz { @haha woo color red background green color @yo { big => blue} }"
  );

Parser.input(Lexer.token, lexbuf);
