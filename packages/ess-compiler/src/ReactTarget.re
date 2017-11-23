[@bs.module "../../../bs-wrapper.js"] external compile : string => string = "";

type output = {
  js_code: string,
  flow_def: string,
  css: string
};

let match_of_styles = (styles, get_name) =>
  Optimizer.ER.fold(
    (states, styles, acc) =>
      "\n| ("
      ++ Optimizer.string_of_state(states)
      ++ ") -> \""
      ++ get_name(styles)
      ++ "\""
      ++ acc,
    styles,
    "\n| _ -> \"\""
  );

let ocaml_impl = (elements, table) => {
  let get_style = (style) =>
    Atomizer.AtomicStyleLookup.class_str_for_style(table, style);
  let ocaml_elements =
    List.map(
      (el: Optimizer.ER.element) => {
        let prop_names =
          List.map(
            (prop) => {
              let (name, _) = prop;
              name
            },
            el.style_types
          );
        "let "
        ++ "c_"
        ++ el.name
        ++ " "
        ++ String.concat(" ", prop_names)
        ++ " = match ("
        ++ String.concat(", ", prop_names)
        ++ ") with "
        ++ match_of_styles(el.style_table, get_style)
      },
      elements
    );
  String.concat("\n", ocaml_elements)
};

let flow_type_of_ast_value_type = (prop_value_type) =>
  switch prop_value_type {
  | Ast.StringEnumType(string_list) =>
    String.concat(" | ", List.map((item) => "\"" ++ item ++ "\"", string_list))
  | Ast.BooleanType => "boolean"
  | Ast.NumberType => "number"
  };

let destructured_props = (prop_names) =>

    ["element"] @
    List.map(
      (x) => {
        let (name, _) = x;
        name
      },
      prop_names
    );

let react_class = (name, prop_names) => {
  let prop_string = String.concat(", ", destructured_props(prop_names));
  {j|exports.$(name) = function $(name)(props) {
  const c = c_$(name)(props);
  const { $(prop_string), ...rest } = props;
  rest.className = rest.className ? rest.className + " " + c : c;
  return React.createElement(props.element, rest);
}|j}
};

let js_react_wrapper = (str, elements: list(Optimizer.ER.element)) =>
  "var React = require('react');\n\n"
  ++ str
  ++ "\n\n"
  ++ String.concat(
       "\n\n",
       List.map(
         (element: Optimizer.ER.element) => {
           let name = element.name;
           react_class(name, element.style_types)
         },
         elements
       )
     )
  ++ "\n";

let flow_interface = (elements) => {
  let prefix = "// @flow\n" ++ "import * as React from 'react';\n\n";
  let interface =
    List.map(
      (el: Optimizer.ER.element) => {
        let prop_names =
          List.map(
            (prop) => {
              let (name, thang) = prop;
              name ++ ": " ++ flow_type_of_ast_value_type(thang)
            },
            el.style_types
          );
        "declare export var "
        ++ el.name
        ++ ": React.StatelessFunctionalComponent<{|\n  "
        ++ String.concat(",\n  ", prop_names)
        ++ "\n|}>;"
      },
      elements
    );
  prefix ++ String.concat("\n", interface)
};

let generate = (elements) : output => {
  let all_styles =
    elements
    |> List.map(
         (el: Optimizer.ER.element) =>
           Optimizer.ER.to_styles_list(el.style_table)
       )
    |> List.flatten;
  let atom_styles =
    all_styles
    |> Array.of_list
    |> Atomizer.StyleHashTable.of_styles
    |> Atomizer.AtomicStyleHashTable.of_declaration_map;
  let table = atom_styles |> Atomizer.AtomicStyleLookup.of_atomic_table;
  let css =
    Atomizer.AtomicStyleHashTable.fold(
      (_, style, str) => {
        let cname =
          Atomizer.AtomicStyleLookup.class_str_for_style(table, style);
        str
        ++ "."
        ++ cname
        ++ " {"
        ++ Styles.StyleSet.to_string(style)
        ++ "}"
        ++ "\n"
      },
      atom_styles,
      ""
    );
  let ocaml = ocaml_impl(elements, table);
  let js = js_react_wrapper(compile(ocaml), elements);
  let flow = flow_interface(elements);
  {js_code: js, css, flow_def: flow}
};
