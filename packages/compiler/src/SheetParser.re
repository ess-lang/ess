module I = Parser.MenhirInterpreter;

type parseResult =
  | Success(Ast.stylesheet)
  | Failure(int, int)
  | UnknownError(string);

let succeed = (sheet: Ast.stylesheet) => Success(sheet);

let stack = (checkpoint) =>
  switch checkpoint {
  | I.HandlingError(env) => I.stack(env)
  | _ => assert false
  };

let state = (checkpoint) : int => {
  let result = Lazy.force(stack(checkpoint));
  switch result {
  | MenhirLib.General.Nil => 0
  | MenhirLib.General.Cons(I.Element(s, _, _, _), _) => I.number(s)
  }
};

let fail = (lexbuf, checkpoint: I.checkpoint(Ast.stylesheet)) => {
  let offset = Lexing.lexeme_start(lexbuf);
  Failure(state(checkpoint), offset)
};

let loop = (lexbuf, result) => {
  let supplier = I.lexer_lexbuf_to_supplier(Lexer.token, lexbuf);
  I.loop_handle(succeed, fail(lexbuf), supplier, result)
};

let process = (token: string) => {
  let lexbuf = Lexing.from_string(token);
  try (loop(lexbuf, Parser.Incremental.input(lexbuf.lex_curr_p))) {
  | Lexer.Error(msg) => UnknownError(msg)
  }
};
