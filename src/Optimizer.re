let rec n_cartesian_product =
  fun
  | [] => [[]]
  | [x, ...xs] => {
      let rest = n_cartesian_product(xs);
      List.concat(List.map((i) => List.map((rs) => [i, ...rs], rest), x))
    };

type prop_state =
  | BoolState(bool)
  | StringState(list(string));

let elements_from_statement = (statement: Ast.statement) =>
  switch statement {
  | ClassDeclaration(name, body) =>
    switch body {
    | ClassBody(x) => []
    }
  | _ => []
  };

let elements_from_stylesheet = (sheet: Ast.stylesheet) =>
  switch sheet {
  | Stylesheet(x) =>
    List.fold_left(
      (acc, statement) => acc @ elements_from_statement(statement),
      [],
      x
    )
  };

type reified_type_value =
  | BooleanVal(bool)
  | StringVal(string);

let expand_type = (prop_type) =>
  switch prop_type {
  | Ast.BooleanType => [BooleanVal(true), BooleanVal(false)]
  | Ast.StringEnumType(str_list) => List.map((str) => StringVal(str), str_list)
  };

module ER = {
  module H =
    Hashtbl.Make(
      {
        type t = list(reified_type_value);
        let hash = Hashtbl.hash;
        let equal = (==);
      }
    );
  include H;
  let from_class_body = (expr_list: list(Ast.class_body_expression)) => {
    let h = H.create(10000);
    /* find the type signature of the element */
    let states =
      List.fold_left(
        (acc, expr) =>
          switch expr {
          | Ast.ClassPropDeclaration(arg, prop_value_type) =>
            let name =
              switch arg {
              | Ast.Argument(str) => str
              };
            [(name, prop_value_type), ...acc]
          | _ => acc
          },
        [],
        expr_list
      );
    /* expand each type into set possible states */
    let expanded =
      List.fold_left(
        (acc, state) => {
          let (_, prop_type) = state;
          let variants = expand_type(prop_type);
          [variants, ...acc]
        },
        [],
        states
      );
    /* calculate all states */
    let possible_states = n_cartesian_product(expanded);
    /* fill hash table with style entries for each possible state */
    List.iter(
      (variants) => H.add(h, variants, Styles.StyleSet.empty),
      possible_states
    );
    /* now go through expressions and fill out table */
    expr_list
    |> List.filter(
         (expr) =>
           switch expr {
           | Ast.StyleExpression(_) => true
           }
       )
    |> List.iter(
         (expr) =>
           switch expr {
           | Ast.StyleExpression(style) =>
             let actual_style: Styles.style =
               AstUtils.style_of_style_ast(style);
             List.iter(
               (state) => {
                 let set = H.find(h, state);
                 let new_set = Styles.StyleSet.add(actual_style, set);
                 H.replace(h, state, new_set)
               },
               possible_states
             )
           }
       );
    h
  };
};
