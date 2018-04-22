type output = {
  js_code: string,
  flow_def: string,
  css: string
};

let compile = (str) => Codegen.to_js(str);

let outputToJs = (x) => x.js_code ++ "" ++ x.flow_def ++ "" ++ x.css;

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
        Printf.sprintf(
          {|let c_%s %s = match (%s) with %s|},
          el.name,
          List.length(prop_names) > 0 ? String.concat(" ", prop_names) : "()",
          String.concat(", ", prop_names),
          match_of_styles(el.style_table, get_style)
        )
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

let destructured_props = (prop_names) => {
  let names =
    ["element"]
    @ List.map(
        (x) => {
          let (name, _) = x;
          name
        },
        prop_names
      );
  List.map((name) => "(" ++ name ++ ")", names)
};

let react_class = (name, prop_names) => {
  let prop_string = String.concat(", ", destructured_props(prop_names));
  Printf.sprintf(
    {|
export function %s(props) {
  const c = c_%s(props);
  const rest = objectWithoutProperties(props, %s);
  rest.className = rest.className
    ? rest.className + " " + c
    : c;
  return React.createElement(props.element, rest);
}|},
    name,
    name,
    prop_string
  )
};

let js_react_wrapper = (str, elements: list(Optimizer.ER.element)) => {
  let asdf =
    String.concat(
      "\n",
      List.map(
        (element: Optimizer.ER.element) => {
          let name = element.name;
          react_class(name, element.style_types)
        },
        elements
      )
    );
  Printf.sprintf(
    {|
function objectWithoutProperties(source, excluded) {
  if (source == null) return {};
  let target = {};
  let sourceKeys = Object.keys(source);
  let key, i;
  for (i = 0; i < sourceKeys.length; i++) {
    key = sourceKeys[i];
    if (excluded.indexOf(key) >= 0) continue;
    target[key] = source[key];
  }
  return target;
}

import React from "react";

%s

%s
|},
    str,
    asdf
  )
};

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
        Printf.sprintf(
          {z|declare export var %s: React.StatelessFunctionalComponent<{|%s|}>;|z},
          el.name,
          String.concat(",\n  ", prop_names)
        )
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
