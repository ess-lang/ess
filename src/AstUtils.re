let style_of_style_ast = (style_node) =>
  switch style_node {
  | Ast.Style(style_property, style_value) =>
    let style =
      switch style_property {
      | Ast.ColorProperty =>
        switch style_value {
        | Ast.ColorNamed(name) => Styles.Color(Styles.RGBA(5, 5, 5, 1))
        | _ => Styles.UnknownStyle
        }
      | _ => Styles.UnknownStyle
      };
    style
  };
