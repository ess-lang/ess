let str = {|Bar [
  @foo = "hello" | "world" | "woah"
  @bar = boolean

  color @foo {
    "world" | "woah" => #0000ff
    "hello" => #000000
  }
]

Baz [
  color #00ff00
]
|};

let result = SheetParser.process(str);

Compiler.maybe_log_error(result);

switch result {
| SheetParser.Success(parsed) =>
  let elements = Optimizer.things_of_stylesheet(parsed);
  List.iter(
    (el: Optimizer.ER.element) => {
      print_endline("========================");
      print_endline("Element: " ++ el.name);
      print_endline(Optimizer.ER.to_string(el.style_table));
      print_endline("")
    },
    elements
  );
  let {css, js_code, flow_def}: ReactTarget.output =
    Compiler.compile_from_ast(parsed);
  print_endline("==== CSS ====");
  print_endline(css);
  print_endline("==== JS ====");
  print_endline(js_code);
  print_endline("==== FLOW ====");
  print_endline(flow_def)
| _ => ()
};
