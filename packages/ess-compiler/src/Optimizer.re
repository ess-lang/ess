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
  | ClassDeclaration(_, body) =>
    switch body {
    | ClassBody(_) => []
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

type state_atom = {
  name: string,
  reval: reified_type_value
};

type state_atom_variants = {
  name: string,
  revals: list(reified_type_value)
};

type clause_type = {
  states: list(state_atom_variants),
  styles: list(Styles.style)
};

let string_of_reified_type_value = (v) =>
  switch v.reval {
  | BooleanVal(bool) => string_of_bool(bool)
  | StringVal(str) => "\"" ++ str ++ "\""
  };

let expand_type = (name, prop_type) =>
  switch prop_type {
  | Ast.BooleanType => [
      {name, reval: BooleanVal(true)},
      {name, reval: BooleanVal(false)}
    ]
  | Ast.StringEnumType(str_list) =>
    List.map((str) => {name, reval: StringVal(str)}, str_list)
  };

let reify_variants_pattern = (name, pattern) =>
  switch pattern {
  | Ast.StringPattern(string_list) => {
      name,
      revals: List.map((str) => StringVal(str), string_list)
    }
  | Ast.BooleanPattern(x) => {name, revals: [BooleanVal(x)]}
  };

let reify_pattern = (name, pattern) =>
  switch pattern {
  | Ast.StringPattern(string_list) =>
    List.map((str) => {name, reval: StringVal(str)}, string_list)
  | Ast.BooleanPattern(x) => [{name, reval: BooleanVal(x)}]
  };

let string_of_state = (state) =>
  String.concat(", ", List.map(string_of_reified_type_value, state));

let string_list_of_args = (args) =>
  Array.of_list(
    List.map(
      (arg) =>
        switch arg {
        | Ast.Argument(arg) => arg
        },
      args
    )
  );

let variants_of_patterns = (args, pattern_list) =>
  List.mapi(
    (index, pattern) => {
      let name = args[index];
      reify_variants_pattern(name, pattern)
    },
    pattern_list
  );

let states_of_patterns = (args, pattern_list) =>
  List.flatten(
    List.mapi(
      (index, pattern) => {
        let name = args[index];
        reify_pattern(name, pattern)
      },
      pattern_list
    )
  );

module ER = {
  module H =
    Hashtbl.Make(
      {
        type t = list(state_atom);
        let hash = Hashtbl.hash;
        let equal = (==);
      }
    );
  include H;
  type element = {
    name: string,
    style_table: H.t(Styles.StyleSet.t),
    style_types: list((string, Ast.prop_value_type))
  };
  let to_styles_list = (h) =>
    H.fold((_, styles, acc) => [styles, ...acc], h, []);
  let to_string = (h) =>
    H.fold(
      (states, styles, acc) =>
        "\n[state]: ("
        ++ string_of_state(states)
        ++ ") [styles]: "
        ++ Styles.StyleSet.to_string(styles)
        ++ acc,
      h,
      ""
    );
  let apply_clauses =
      (h, possible_states: list(list(state_atom)), style_clauses) =>
    List.iter(
      (state: list(state_atom)) => {
        let relevant_clauses =
          style_clauses
          |> List.filter(
               (style_clause) =>
                 style_clause.states
                 |> List.for_all(
                      (substate: state_atom_variants) =>
                        substate.revals
                        |> List.exists(
                             (reval) => {
                               let part = {name: substate.name, reval};
                               List.exists((inner) => inner == part, state)
                             }
                           )
                    )
             );
        /* reverse order, should have warning if length > 1 */
        relevant_clauses
        |> List.rev
        |> List.iter(
             (clause) => {
               let set = H.find(h, state);
               let new_set =
                 List.fold_left(
                   (acc, style) => Styles.StyleSet.add_style(style, acc),
                   set,
                   clause.styles
                 );
               H.replace(h, state, new_set)
             }
           )
      },
      possible_states
    );
  let from_class_body =
      (name: string, expr_list: list(Ast.class_body_expression)) => {
    let h = H.create(1);
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
          let (name, prop_type) = state;
          let variants = expand_type(name, prop_type);
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
           | Ast.MatchValueExpression(style_prop, args_list, clauses_list) =>
             let args = string_list_of_args(args_list);
             let style_clauses =
               List.fold_left(
                 (acc, clause) =>
                   switch clause {
                   | Ast.MatchValueClause(pattern_list, style_val) =>
                     let style =
                       AstUtils.style_of_style_pair(style_prop, style_val);
                     let states = variants_of_patterns(args, pattern_list);
                     [{states, styles: [style]}, ...acc]
                   },
                 [],
                 clauses_list
               );
             apply_clauses(h, possible_states, style_clauses)
           | Ast.MatchBlockExpression(args_list, clauses_list) =>
             let args = string_list_of_args(args_list);
             let style_clauses =
               List.fold_left(
                 (acc, clause) =>
                   switch clause {
                   | Ast.MatchBlockClause(pattern_list, style_list) =>
                     let styles =
                       List.map(
                         (style) => AstUtils.style_of_style_ast(style),
                         style_list
                       );
                     let states = variants_of_patterns(args, pattern_list);
                     [{states, styles}, ...acc]
                   },
                 [],
                 clauses_list
               );
             apply_clauses(h, possible_states, style_clauses)
           | _ => () /* todo */
           }
       );
    {name, style_types: states, style_table: h}
  };
};

/* type element = {
     name: string,
     styles: ER.t(Styles.StyleSet.t)
   }; */
let things_of_stylesheet = (stylesheet_node) =>
  switch stylesheet_node {
  | Ast.Stylesheet(statements) =>
    List.fold_left(
      (acc, statement) =>
        switch statement {
        | Ast.ClassDeclaration(name, body) =>
          switch body {
          | Ast.ClassBody(expr_list) => [
              ER.from_class_body(name, expr_list),
              ...acc
            ]
          }
        | _ => acc
        },
      [],
      statements
    )
  };
