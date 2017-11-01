type stylesheet = list(statement)
and statement =
  | ClassDeclaration(string, class_body)
  | VariableDeclaration(string, style_value)
and class_body =
  | ClassBody(list(class_prop_declaration), list(class_body_expression))
and class_body_expression =
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
and class_prop_declaration =
  | ClassPropDeclaration(string, string)
and argument =
  | Argument(string)
and style_property =
  | ColorProperty
  | BackgroundProperty
  | PaddingProperty
  | BorderProperty
and style_value =
  | Px(float)
  | Em(float)
  | StringLiteral(string)
  | VariableReference
  | ValueExpression
and pattern =
  | NumberPattern(int)
  | NumberRangePattern(int, int)
  | StringPattern(list(string));
