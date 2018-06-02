type loc = (Lexing.position, Lexing.position);

[@deriving visitors({variety: "iter"})]
type stylesheet =
  | Stylesheet(list(statement))
and statement =
  | ElementDeclaration([@opaque] loc, string, list(parameter), block)
  | VariableDeclaration([@opaque] loc)
and block =
  | Block([@opaque] loc, list(rule))
and rule =
  | AttributeRule([@opaque] loc, string, expression)
  | CompositionRule([@opaque] loc, string)
  | MatchRule([@opaque] loc, list(parameter), list(match_rule_clause))
and expression =
  | MatchExpression([@opaque] loc, list(parameter), list(match_expression_clause))
  | RecordExpression([@opaque] loc, list(record_field))
  | LiteralExpression([@opaque] loc)
  | UnknownExpression([@opaque] loc)
and record_field =
  | RecordField([@opaque] loc, string, literal)
/* need to type check named expression assignment. Must only be primitive value */
and literal =
  | ReferenceLiteral([@opaque] loc)
  | TextLiteral([@opaque] loc)
  | HexLiteral([@opaque] loc)
  | PixelLiteral([@opaque] loc)
  | PercentLiteral([@opaque] loc)
  | NumberLiteral([@opaque] loc)
and match_rule_clause =
  | MatchRuleClause([@opaque] loc, list(pattern), block)
and match_expression_clause =
  | MatchExpressionClause([@opaque] loc, list(pattern), expression)
and prop_value_type =
  | StringEnumType([@opaque] loc, list(string))
  | BooleanType([@opaque] loc)
  | NumberType([@opaque] loc)
and parameter =
  | Parameter([@opaque] loc, string)
and pattern =
  | NumberPattern([@opaque] loc, int)
  | NumberRangePattern([@opaque] loc, (int, option(int)))
  | BooleanPattern([@opaque] loc, bool)
  | StringPattern([@opaque] loc, list(string))
  | FallthroughPattern([@opaque] loc);
