type color =
  | RGBA int int int int;

let string_of_color color =>
  switch color {
  | RGBA r g b a =>
    "rgba("
    ^ string_of_int r
    ^ ", "
    ^ string_of_int g
    ^ ", "
    ^ string_of_int b
    ^ ", "
    ^ string_of_int a
    ^ ")"
  };

type length =
  | Px float
  | Em float;

let string_of_length length =>
  switch length {
  | Px n => string_of_float n ^ "px"
  | Em n => string_of_float n ^ "em"
  };

type direction =
  | Top
  | Right
  | Bottom
  | Left;

let string_of_direction direction =>
  switch direction {
  | Top => "top"
  | Right => "right"
  | Bottom => "bottom"
  | Left => "left"
  };

type borderStyle =
  | None
  | Solid;

let string_of_border_style borderStyle =>
  switch borderStyle {
  | None => "none"
  | Solid => "solid"
  };

type declaration =
  | Color color
  | Border direction length borderStyle color
  | Background color;

let string_of_declaration decl =>
  switch decl {
  | Color c => "(color: " ^ string_of_color c ^ ")"
  | Border dir len bs c =>
    "(border: "
    ^ string_of_direction dir
    ^ ", "
    ^ string_of_length len
    ^ ", "
    ^ string_of_border_style bs
    ^ ", "
    ^ string_of_color c
    ^ ")"
  | Background c => "(background: " ^ string_of_color c ^ ")"
  };

/* Set of unique declarations, constitutes a style */
module DeclarationSet = {
  module S =
    Set.Make {
      type t = declaration;
      let compare = compare;
    };
  include S;
  let to_string set =>
    String.concat ",\n  " (List.map string_of_declaration (S.elements set));
};

let create_style = DeclarationSet.of_list;

module IntSet =
  Set.Make {
    type t = int;
    let compare = compare;
  };

module DeclarationMap = {
  module M =
    Hashtbl.Make {
      type t = declaration;
      let hash = Hashtbl.hash;
      let equal = (==);
    };
  include M;
  let of_styles styles_array => {
    let map = M.create 10000;
    Array.iteri
      (
        fun index decls =>
          DeclarationSet.iter
            (
              fun decl =>
                if (M.mem map decl) {
                  let set = M.find map decl;
                  M.replace map decl (IntSet.add index set)
                } else {
                  M.add map decl (IntSet.singleton index)
                }
            )
            decls
      )
      styles_array;
    map
  };
};

module AtomicDeclarationMap = {
  module M =
    Hashtbl.Make {
      type t = IntSet.t;
      let hash = Hashtbl.hash;
      let equal = (==);
    };
  include M;
  let of_declaration_map map => {
    let atoms = M.create 10000;
    DeclarationMap.iter
      (
        fun decl stylesSet =>
          if (M.mem atoms stylesSet) {
            let set = M.find atoms stylesSet;
            M.replace atoms stylesSet (DeclarationSet.add decl set)
          } else {
            M.add atoms stylesSet (DeclarationSet.singleton decl)
          }
      )
      map;
    atoms
  };
};

/* styles to atomize */
let styles = [|
  create_style [Color (RGBA 255 0 0 255)],
  create_style [Background (RGBA 0 255 0 255)],
  create_style [
    Color (RGBA 255 0 0 255),
    Background (RGBA 0 255 0 255),
    Border Top (Px 2.0) Solid (RGBA 255 0 0 255),
    Border Bottom (Px 2.0) Solid (RGBA 255 0 0 255)
  ],
  create_style [Color (RGBA 255 255 0 255), Background (RGBA 255 255 0 255)]
|];

let atoms =
  styles |> DeclarationMap.of_styles |> AtomicDeclarationMap.of_declaration_map;

AtomicDeclarationMap.iter
  (fun _ decls => Js.log ("[ " ^ DeclarationSet.to_string decls ^ " ]")) atoms;
