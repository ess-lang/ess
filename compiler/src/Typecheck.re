type numy =
  | Foo
  | Bar
  | Baz
  ;

type thingy = {
  asdf: numy
};

let typecheck = (e: Ast.stylesheet) : int => {
  let v = {
    as _;
    val mutable count = 0;
    pub count = count;
    inherit class Ast.iter(_) as super;
    pub! visit_AttributeRule = (env, e0, e1) => {
      count = count + 1;
      print_endline(e1);
      let yolo =  switch (e1) {
        | "foo" => Baz
        | "blah" => Bar
        | _ => Foo
      };

      super#visit_AttributeRule({asdf: yolo}, e0, e1);
    };
    pub! visit_expression = (env, x) => {

      /* pattern match against value types, based on env */
      switch (env.asdf) {
        | Foo => print_endline("expression: Foo")
        | Bar => print_endline("expression: Bar")
        | Baz => print_endline("expression: Baz")
      };
      super#visit_expression(env, x);
    }
  };
  v#visit_stylesheet({asdf: Foo}, e);
  v#count;
};
