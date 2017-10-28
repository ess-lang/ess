type classProptype =
  | Proptype(string, string);

type classBlockItem =
  | Declaration(string, string);

type classBody =
  | ClassBody(list(classProptype), list(classBlockItem));

type statement =
  | ClassDeclaration(string, classBody)
  | VariableDeclaration;

type program = list(statement);
