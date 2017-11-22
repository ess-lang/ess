let style_of_style_pair = (style_property, style_value) =>
  switch style_property {
  | Ast.ColorProperty =>
    switch style_value {
    | Ast.ColorNamed(name) => Styles.Color(Styles.RGBA(5, 5, 5, 1))
    | Ast.ColorRGB(rgb) =>
      let (r, g, b) = rgb;
      Styles.Color(Styles.RGBA(r, g, b, 1))
    | _ => Styles.UnknownStyle
    }
  | Ast.BackgroundProperty =>
    switch style_value {
    | Ast.ColorRGB(rgb) =>
      let (r, g, b) = rgb;
      Styles.Background(Styles.RGBA(r, g, b, 1))
    | _ => Styles.UnknownStyle
    }
  | _ => Styles.UnknownStyle
  };

let style_of_style_ast = (style_node) =>
  switch style_node {
  | Ast.Style(style_property, style_value) =>
    style_of_style_pair(style_property, style_value)
  };
