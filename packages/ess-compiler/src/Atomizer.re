open Styles;

type counter = {
  unique: int,
  msb: int,
  offset: int,
  power: int
};

let is_even = (n) => n mod 2 == 0;

let pow = (base, exponent) =>
  if (exponent < 0) {
    invalid_arg("exponent can not be negative")
  } else {
    let rec aux = (accumulator, base) =>
      fun
      | 0 => accumulator
      | 1 => base * accumulator
      | e when is_even(e) => aux(accumulator, base * base, e / 2)
      | e => aux(base * accumulator, base * base, (e - 1) / 2);
    aux(1, base, exponent)
  };

let increment = (count: counter) => {
  let virtualCount = count.unique + count.offset;
  let unique = count.unique + 1;
  if (virtualCount == count.msb) {
    let new_power = count.power + 1;
    {
      msb: pow(36, new_power) - 1,
      offset: count.offset + (count.msb + 1) * 9,
      unique,
      power: new_power
    }
  } else {
    {unique, power: count.power, msb: count.msb, offset: count.offset}
  }
};

let to_base = (b, v) => {
  let rec to_base' = (a, v) =>
    if (v == 0) {
      a
    } else {
      to_base'([v mod b, ...a], v / b)
    };
  to_base'([], v)
};

let to_alpha_digit = (n: int) =>
  String.make(
    1,
    if (n < 10) {
      char_of_int(n + int_of_char('0'))
    } else {
      char_of_int(n + int_of_char('a') - 10)
    }
  );

let to_alpha_digits = (ds) =>
  switch (List.length(ds)) {
  | 0 => "0"
  | _ => List.fold_left((str, i) => str ++ to_alpha_digit(i), "", ds)
  };

let int_to_base36 = (int) => to_base(36, int) |> to_alpha_digits;

let count_to_class_name = (count: counter) =>
  int_to_base36(count.unique + count.offset);

module IntSet =
  Set.Make(
    {
      type t = int;
      let compare = compare;
    }
  );

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

exception SomeAtomicError;

module StringSet =
  Set.Make(
    {
      type t = string;
      let compare = compare;
    }
  );

module AtomicStyleLookup = {
  module M =
    Hashtbl.Make(
      {
        type t = style;
        let hash = Hashtbl.hash;
        let equal = (==);
      }
    );
  include M;
  let class_str_for_style = (lookup_table, style_set) => {
    let woo =
      StyleSet.fold(
        (style, set) => StringSet.add(M.find(lookup_table, style), set),
        style_set,
        StringSet.empty
      );
    String.concat(" ", StringSet.elements(woo))
  };
  let of_atomic_table = (tbl) => {
    let initial_counter = {
      unique: 0,
      offset: 10, /* skip 0-9 */
      msb: 35,
      power: 1
    };
    let lookup_table = M.create(10000);
    AtomicStyleHashTable.fold(
      (intSet, stylesSet, count) => {
        stylesSet
        |> StyleSet.iter(
             (style) =>
               if (M.mem(lookup_table, style)) {
                 raise
                   (SomeAtomicError)
                   /* style should not exist already in table */
               } else {
                 M.add(lookup_table, style, count_to_class_name(count))
               }
           );
        increment(count)
      },
      tbl,
      initial_counter
    );
    lookup_table
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
