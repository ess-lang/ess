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

ReactTarget.generate(elements);
