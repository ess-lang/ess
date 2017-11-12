type color =
  | RGBA(int, int, int, int)
and length =
  | Px(float)
  | Em(float)
and direction =
  | Top
  | Right
  | Bottom
  | Left
and borderStyle =
  | None
  | Solid
and declaration =
  | Color(color)
  | Border(direction, length, borderStyle, color)
  | Background(color);

let string_of_color = (color) =>
  switch color {
  | RGBA(r, g, b, a) =>
    let r_ = string_of_int(r);
    let g_ = string_of_int(g);
    let b_ = string_of_int(b);
    let a_ = string_of_int(a);
    {j|rgba($(r_),$(g_),$(b_),$(a_))|j}
  };

/*
 Returns a CSS-compatible string representation of a float.

 string_of_float(1.2)     => "1.2"
 css_string_of_float(1.2) => "1.2"

 string_of_float(1.0)     => "1."
 css_string_of_float(1.0) => "1"
 */
let css_string_of_float = (float) => {
  let as_string = string_of_float(float);
  let is_integer = as_string.[String.length(as_string) - 1] == '.';
  is_integer ?
    {
      let length = String.length(as_string) - 1;
      String.sub(as_string, 0, length)
    } :
    as_string
};

let string_of_length = (length) =>
  switch length {
  | Px(n) => css_string_of_float(n) ++ "px"
  | Em(n) => css_string_of_float(n) ++ "em"
  };

let string_of_direction = (direction) =>
  switch direction {
  | Top => "top"
  | Right => "right"
  | Bottom => "bottom"
  | Left => "left"
  };

let string_of_border_style = (borderStyle) =>
  switch borderStyle {
  | None => "none"
  | Solid => "solid"
  };

let string_of_declaration = (decl) =>
  switch decl {
  | Color(c) => "(color: " ++ (string_of_color(c) ++ ")")
  | Border(dir, len, bs, c) =>
    let dir_ = string_of_direction(dir);
    let len_ = string_of_length(len);
    let bs_ = string_of_border_style(bs);
    let c_ = string_of_color(c);
    {j|(border: $(dir_) $(len_) $(bs_) $(c_))|j}
  | Background(c) => "(background: " ++ (string_of_color(c) ++ ")")
  };

/* Set of unique declarations, constitutes a style */
module DeclarationSet = {
  module S =
    Set.Make(
      {
        type t = declaration;
        let compare = compare;
      }
    );
  include S;
  let to_string = (set) =>
    String.concat(",\n  ", List.map(string_of_declaration, S.elements(set)));
};

let create_style = DeclarationSet.of_list;

module IntSet =
  Set.Make(
    {
      type t = int;
      let compare = compare;
    }
  );

module DeclarationMap = {
  module M =
    Hashtbl.Make(
      {
        type t = declaration;
        let hash = Hashtbl.hash;
        let equal = (==);
      }
    );
  include M;
  let of_styles = (styles_array) => {
    let map = M.create(10000);
    Array.iteri(
      (index, decls) =>
        DeclarationSet.iter(
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

module AtomicDeclarationMap = {
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
    DeclarationMap.iter(
      (decl, stylesSet) =>
        if (M.mem(atoms, stylesSet)) {
          let set = M.find(atoms, stylesSet);
          M.replace(atoms, stylesSet, DeclarationSet.add(decl, set))
        } else {
          M.add(atoms, stylesSet, DeclarationSet.singleton(decl))
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
  styles |> DeclarationMap.of_styles |> AtomicDeclarationMap.of_declaration_map;

AtomicDeclarationMap.iter(
  (_, decls) => Js.log("[ " ++ (DeclarationSet.to_string(decls) ++ " ]")),
  atoms
);
