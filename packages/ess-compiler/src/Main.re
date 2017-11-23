let str = Node_fs.readFileAsUtf8Sync("./src/test.ess");

let lexbuf = Lexing.from_string(str);

let parsed = Parser.input(Lexer.token, lexbuf);

let elements = Optimizer.things_of_stylesheet(parsed);

List.iter(
  (el: Optimizer.ER.element) => {
    Js.log("========================");
    Js.log("Element: " ++ el.name);
    Js.log(Optimizer.ER.to_string(el.style_table));
    Js.log("")
  },
  elements
);

let {css, js_code, flow_def}: ReactTarget.output =
  Compiler.compile_from_ast(parsed);

Js.log("==== CSS ====");

Js.log(css);

Js.log("==== JS ====");

Js.log(js_code);

Js.log("==== FLOW ====");

Js.log(flow_def);
