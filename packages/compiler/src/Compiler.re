let compile_from_ast = (ast) : ReactTarget.output => {
  let elements = Optimizer.things_of_stylesheet(ast);
  ReactTarget.generate(elements)
};

let maybe_log_error = (result) =>
  switch result {
  | SheetParser.Success(ast) => Some(compile_from_ast(ast))
  | SheetParser.Failure(state, offset) =>
    let msg = ParseErrors.message(state);
    Js.log({j|Error: $(msg) at offset: $(offset)|j});
    None
  | SheetParser.UnknownError(msg) =>
    Js.log({j|Error: $(msg)|j});
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
