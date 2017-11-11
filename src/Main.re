let str = Node_fs.readFileAsUtf8Sync("./src/test.ess");

let lexbuf = Lexing.from_string(str);

let parsed = Parser.input(Lexer.token, lexbuf);

Js.log @@ Js.Json.stringifyAny(parsed);
/* Js.log @@ Ast.string_of_stylesheet(parsed); */
