type classProptype =
  | Proptype(string, string);

type patternDeclaration =
  | PatternDeclaration(string, string);

type declarationValue =
  | ValueLiteral(string)
  | MatchValue(string, list(patternDeclaration));

type classBlockItem =
  | Declaration(string, declarationValue);

type classBody =
  | ClassBody(list(classProptype), list(classBlockItem));

type statement =
  | ClassDeclaration(string, classBody)
  | VariableDeclaration;

type program = list(statement);
