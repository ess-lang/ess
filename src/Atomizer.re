open Styles;

module StyleHashTable = {
  module M =
    Hashtbl.Make(
      {
        type t = style;
        let hash = Hashtbl.hash;
        let equal = (==);
      }
    );
  include M;
  let of_styles = (styles_array) => {
    let map = M.create(10000);
    Array.iteri(
      (index, decls) =>
        StyleSet.iter(
          (decl) =>
            if (M.mem(map, decl)) {
              let set = M.find(map, decl);
              M.replace(map, decl, IntSet.add(index, set))
            } else {
              M.add(map, decl, IntSet.singleton(index))
            },
          decls
        ),
      styles_array
    );
    map
  };
};

module AtomicStyleHashTable = {
  module M =
    Hashtbl.Make(
      {
        type t = IntSet.t;
        let hash = Hashtbl.hash;
        let equal = (==);
      }
    );
  include M;
  let of_declaration_map = (map) => {
    let atoms = M.create(10000);
    StyleHashTable.iter(
      (decl, stylesSet) =>
        if (M.mem(atoms, stylesSet)) {
          let set = M.find(atoms, stylesSet);
          M.replace(atoms, stylesSet, StyleSet.add(decl, set))
        } else {
          M.add(atoms, stylesSet, StyleSet.singleton(decl))
        },
      map
    );
    atoms
  };
};

/* styles to atomize */
let styles = [|
  create_style([Color(RGBA(255, 0, 0, 255))]),
  create_style([Background(RGBA(0, 255, 0, 255))]),
  create_style([
    Color(RGBA(255, 0, 0, 255)),
    Background(RGBA(0, 255, 0, 255)),
    Border(Top, Px(2.0), Solid, RGBA(255, 0, 0, 255)),
    Border(Bottom, Px(2.5), Solid, RGBA(255, 0, 0, 255))
  ]),
  create_style([
    Color(RGBA(255, 255, 0, 255)),
    Background(RGBA(255, 255, 0, 255))
  ])
|];

let atoms =
  styles |> StyleHashTable.of_styles |> AtomicStyleHashTable.of_declaration_map;

AtomicStyleHashTable.iter(
  (_, decls) => Js.log("[ " ++ (StyleSet.to_string(decls) ++ " ]")),
  atoms
);
