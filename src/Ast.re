type stylesheet =
  | Stylesheet(list(statement))
and statement =
  | ClassDeclaration(string, class_body)
  | VariableDeclaration(string, style_value)
and class_body =
  | ClassBody(list(class_body_expression))
and class_body_expression =
  | ClassPropDeclaration(argument, prop_value_type)
  | StyleExpression(style)
  | MatchBlockExpression(list(argument), list(match_block_clause))
  | MatchValueExpression(
      style_property,
      list(argument),
      list(match_value_clause)
    )
and match_block_clause =
  | MatchBlockClause(list(pattern), list(style))
and match_value_clause =
  | MatchValueClause(list(pattern), style_value)
and style =
  | Style(style_property, style_value)
and prop_value_type =
  | StringEnumType(list(string))
  | BooleanType
  | NumberType
and argument =
  | Argument(string)
and style_property =
  | ColorProperty
  | BackgroundProperty
  | PaddingProperty
  | BorderProperty
  | LineHeightProperty
  | UnknownProperty(string)
and style_value =
  | Px(float)
  | Em(float)
  | StringLiteral(string)
  | ColorRGB((int, int, int))
  | ColorNamed(string)
  | VariableReference
  | ValueExpression
and pattern =
  | NumberPattern(int)
  | NumberRangePattern((int, option(int)))
  | StringPattern(list(string))
  | FallthroughPattern;
/* let string_of_statement = (n) =>
     switch n {
     | ClassDeclaration(x, y) => "classdecl"
     | VariableDeclaration(x, y) => "variable"
     };

   let rec string_of_statement_list = (n) =>
     switch n {
     | [] => "empty"
     | [a, ...rest] => string_of_statement(a) ++ string_of_statement_list(rest)
     };

   let string_of_stylesheet = (n) =>
     switch n {
     | Stylesheet(x) => string_of_statement_list(x)
     }; */
