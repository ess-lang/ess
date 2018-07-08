let maybe_log_error = result =>
  switch (result) {
  | SheetParser.Success(ast) => print_endline("success")
  | SheetParser.Failure(state, pos) =>
    let msg = "FAKE";
    /* let msg = ParseErrors.message(state); */
    let line = pos.pos_lnum;
    let col = pos.pos_cnum - pos.pos_bol + 1;
    print_endline(
      "Error: "
      ++ msg
      ++ " at "
      ++ string_of_int(line)
      ++ ":"
      ++ string_of_int(col),
    );
  | SheetParser.UnknownError(msg) => print_endline("Error: " ++ msg)
  };
