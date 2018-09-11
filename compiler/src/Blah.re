let str = {|
Baz100 (
  @foo: boolean
  @notcool: "evenmore" | "lame"
) [
  Other1
  ,
  @foo {
    true => [Other2,Other3],
    false => [
      Other4
      Other5
    ],
    (true, false) => [Other6],
  },
  Other7,
]

Baz [
  blah asdf
  foo (@foo {
    true => @foo {
      true => green
    }
  })
  Other8,
  arbitrary (
    hello: world,
    hello2: world2,
    hello3: world3

  )
  asdf (


  )
]

Bar [
  background @foo {
    true => red
  }
]

|};

let result = SheetParser.process(str);

Compiler.maybe_log_error(result);

switch (result) {
| SheetParser.Success(parsed) =>
  /* let yo = Typecheck.typecheck(parsed); */
  let thang = Compilation.resolve(parsed);
  print_endline(string_of_int(Compilation.SymbolTable.length(thang.table)));
  /* print_endline(string_of_int(yo)); */
  print_endline("dope!");
| _ => ()
};
