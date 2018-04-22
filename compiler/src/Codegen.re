open Whole_compiler_lib;

let ppf = Format.err_formatter;

let js_of_ocaml = (program) => {
  let ast =
    Ocaml_parse.parse_implementation_from_string(program)
    |> Pparse.apply_rewriters_str(~tool_name=Js_config.tool_name);
  let sourcefile = "Foo";
  let output_prefix = "Foo";
  let modulename = "Foo";
  let env = Compmisc.initial_env();
  let (typedtree, coercion, finalenv, current_signature) =
    ast
    |> Typemod.type_implementation_more(
         sourcefile,
         output_prefix,
         modulename,
         env
       );
  let lam = Translmod.transl_implementation(modulename, (typedtree, coercion));
  let prog =
    Lam_compile_main.compile(
      "asdf",
      "asdfsdf",
      finalenv,
      current_signature,
      lam
    );
  let buf = Buffer.create(0);
  Js_dump_program.pp_deps_program(
    ~output_prefix,
    Es6,
    prog,
    Ext_pp.from_buffer(buf)
  );
  Buffer.contents(buf)
};

let to_js = (program) => {
  Clflags.dont_write_files := true;
  Js_config.no_version_header := true;
  Compmisc.init_path(false);
  try (js_of_ocaml(program)) {
  | x =>
    Location.report_exception(ppf, x);
    exit(2)
  }
};
