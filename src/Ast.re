type classProptype =
  | Proptype(string, string);

type patternDeclaration =
  | PatternDeclaration(string, string);

type styleValue =
  | ValueLiteral(string)
  | MatchValue(string, list(patternDeclaration));

type classBlockItem =
  | Style(string, styleValue);

type classBody =
  | ClassBody(list(classProptype), list(classBlockItem));

type statement =
  | ClassDeclaration(string, classBody)
  | VariableDeclaration;

type program = list(statement);
