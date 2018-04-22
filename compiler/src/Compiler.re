let compile_from_ast = (ast) : ReactTarget.output => {
  let elements = Optimizer.things_of_stylesheet(ast);
  ReactTarget.generate(elements)
};

let maybe_log_error = (result) =>
  switch result {
  | SheetParser.Success(ast) => Some(compile_from_ast(ast))
  | SheetParser.Failure(state, pos) =>
    let msg = ParseErrors.message(state);
    let line = pos.pos_lnum;
    let col = pos.pos_cnum - pos.pos_bol + 1;
    print_endline(
      "Error: "
      ++ msg
      ++ " at "
      ++ string_of_int(line)
      ++ " : "
      ++ string_of_int(col)
    );
    None
  | SheetParser.UnknownError(msg) =>
    print_endline("Error: " ++ msg);
    None
  };

let compile = (str: string) : option(ReactTarget.output) => {
  let result = SheetParser.process(str);
  maybe_log_error(result)
};

let compile_js = (str: string) =>
  switch (compile(str)) {
  | Some(ast) => ReactTarget.outputToJs(ast)
  | None =>
    ReactTarget.outputToJs({js_code: "error", flow_def: "error", css: "error"})
  };
