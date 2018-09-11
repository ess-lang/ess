open SheetParser;
open Compilation;

let read_file = filename =>
  Lwt_io.with_file(~mode=Lwt_io.Input, filename, Lwt_io.read);
let write_file = (filename, contents) =>
  Lwt_io.with_file(~mode=Lwt_io.Output, filename, ch =>
    Lwt_io.write(ch, contents)
  );

let print_files = (files: list(string), dir: string) => {
  let%lwt sources = Lwt_list.map_s(read_file, files);
  let results = List.map(str => SheetParser.process(str), sources);
  List.iter(
    result =>
      switch (result) {
      | SheetParser.Success(parsed) =>
        /* let yo = Typecheck.typecheck(parsed); */
        let resolved = Compilation.resolve(parsed);
        print_endline(
          string_of_int(Compilation.SymbolTable.length(resolved.table)),
        );
      /* print_endline(string_of_int(yo)); */
      /* print_endline("dope!"); */
      | _ => ()
      },
    results,
  );

  Lwt_list.map_s(
    file => write_file(file ++ ".js", "// the js implementation\n"),
    files,
  );
};

let foo = Watchman.run_query(print_files);
