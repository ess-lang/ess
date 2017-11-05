let str = Node_fs.readFileAsUtf8Sync("./src/test.ess");

let lexbuf = Lexing.from_string(str);

Parser.input(Lexer.token, lexbuf);
