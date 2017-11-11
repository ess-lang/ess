let list_maybe = (l) =>
  switch l {
  | Some(l) => l
  | None => []
  };

let string_to_property = (str) =>
  switch str {
  | "color" => Ast.ColorProperty
  | "line-height" => Ast.LineHeightProperty
  | "padding" => Ast.PaddingProperty
  | "border" => Ast.BorderProperty
  | "background" => Ast.BackgroundProperty
  | _ => Ast.UnknownProperty(str)
  };
