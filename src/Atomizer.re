type color =
  | RGBA int int int int;

type length =
  | Px float
  | Em float;

type direction =
  | Top
  | Right
  | Bottom
  | Left;

type borderStyle =
  | None
  | Solid;

type declaration =
  | Color color
  | Border direction length borderStyle color
  | Background color;

let string_of_declaration decl =>
  switch decl {
  | Color _ => "(color: _)"
  | Border _ => "(border: _)"
  | Background _ => "(background: _)"
  };

/* Set of unique declarations, constitutes a style */
module DeclarationSet = {
  module S =
    Set.Make {
      type t = declaration;
      let compare = compare;
    };
  /* let set_of_list list =>
     List.fold_left (fun set elem => S.add elem set) S.empty list; */
  let to_string set =>
    String.concat ", " (List.map string_of_declaration (S.elements set));
};

let createStyle = DeclarationSet.S.of_list;

/* Will hold map from declarations to set of styles that contain it */
module DeclarationMap =
  Hashtbl.Make {
    type t = declaration;
    let hash = Hashtbl.hash;
    let equal = (==);
  };

module IntSet =
  Set.Make {
    type t = int;
    let compare = compare;
  };

/* Holds stuff */
module UniqueMap =
  Hashtbl.Make {
    type t = IntSet.t;
    let hash = Hashtbl.hash;
    let equal = (==);
  };

/* styles to atomize */
let styles = [|
  createStyle [Color (RGBA 255 0 0 255)],
  createStyle [Background (RGBA 0 255 0 255)],
  createStyle [
    Color (RGBA 255 0 0 255),
    Background (RGBA 0 255 0 255),
    Border Top (Px 2.0) Solid (RGBA 255 0 0 255),
    Border Bottom (Px 2.0) Solid (RGBA 255 0 0 255)
  ],
  createStyle [Color (RGBA 255 255 0 255), Background (RGBA 255 255 0 255)]
|];

let map = DeclarationMap.create 10000;

Array.iteri
  (
    fun index decls =>
      DeclarationSet.S.iter
        (
          fun decl =>
            if (DeclarationMap.mem map decl) {
              let set = DeclarationMap.find map decl;
              let boom = IntSet.add index set;
              DeclarationMap.replace map decl boom;
              ()
            } else {
              DeclarationMap.add map decl (IntSet.singleton index)
            }
        )
        decls
  )
  styles;

let atoms = UniqueMap.create 10000;

DeclarationMap.iter
  (
    fun decl stylesSet =>
      if (UniqueMap.mem atoms stylesSet) {
        let set = UniqueMap.find atoms stylesSet;
        let boom = DeclarationSet.S.add decl set;
        UniqueMap.replace atoms stylesSet boom;
        ()
      } else {
        UniqueMap.add atoms stylesSet (DeclarationSet.S.singleton decl)
      }
  )
  map;

UniqueMap.iter
  (fun _ b => Js.log ("atom: " ^ DeclarationSet.to_string b)) atoms;
