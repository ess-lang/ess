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
and style =
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

let string_of_style = (style) =>
  switch style {
  | Color(c) => "(color: " ++ (string_of_color(c) ++ ")")
  | Border(dir, len, bs, c) =>
    let dir_ = string_of_direction(dir);
    let len_ = string_of_length(len);
    let bs_ = string_of_border_style(bs);
    let c_ = string_of_color(c);
    {j|(border: $(dir_) $(len_) $(bs_) $(c_))|j}
  | Background(c) => "(background: " ++ (string_of_color(c) ++ ")")
  };

/* Set of unique styles, constitutes a style */
module StyleSet = {
  module S =
    Set.Make(
      {
        type t = style;
        let compare = compare;
      }
    );
  include S;
  let to_string = (set) =>
    String.concat(",\n  ", List.map(string_of_style, S.elements(set)));
};

let create_style = StyleSet.of_list;

module IntSet =
  Set.Make(
    {
      type t = int;
      let compare = compare;
    }
  );
