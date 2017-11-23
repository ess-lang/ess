let compile_from_ast = (ast) : ReactTarget.output => {
  let elements = Optimizer.things_of_stylesheet(ast);
  ReactTarget.generate(elements)
};

let compile = (str: string) : ReactTarget.output => {
  let lexbuf = Lexing.from_string(str);
  let parsed = Parser.input(Lexer.token, lexbuf);
  compile_from_ast(parsed)
};
