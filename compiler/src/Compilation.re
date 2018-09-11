type reference_kind =
  | Element;
type symbol_key = {
  name: string,
  kind: reference_kind,
};

module SymbolTable = {
  module H =
    Hashtbl.Make({
      type t = symbol_key;
      let hash = Hashtbl.hash;
      let equal = (==);
    });
  include H;
};

type traverse_state = {table: SymbolTable.t(Eir.node)};

let resolve = (e: Ast.stylesheet) : traverse_state => {
  let state = {table: SymbolTable.create(1000)};
  let v = {
    as _;
    inherit class Ast.iter(_) as super;
    pub! visit_ElementDeclaration = (env, loc, name, param_list, block) => {
      print_endline("an element:");
      print_endline(name);
      let interface = Eir.SM.singleton("foo", Eir.NumberType);

      List.iter(
        param => {
          let param_name =
            switch (param) {
            | Ast.Parameter(_, x) => x
            };
          Eir.SM.add(param_name, Eir.NumberType, interface);
          ();
        },
        param_list,
      );

      SymbolTable.add(
        env.table,
        {name, kind: Element},
        Eir.Element({interface: interface}),
      );

      super#visit_ElementDeclaration(env, loc, name, param_list, block);
    }
  };
  v#visit_stylesheet(state, e);
  state;
};
