type io = {
  input: Lwt_io.channel(Lwt_io.input),
  output: Lwt_io.channel(Lwt_io.output),
};

[@deriving yojson]
type sockname_response = {
  version: string,
  sockname: string,
}
and watch_project_response = {
  watcher: string,
  watch: string,
  relative_path: [@default ""] string,
  version: string,
}
and query_response = {
  files: list(string),
  clock: string,
  is_fresh_instance: bool,
  version: string,
}
and subscribe_response = {
  clock: string,
  subscribe: string,
  version: string,
}
and subscription_message = {
  unilateral: bool,
  subscription: string,
  root: string,
  files: list(string),
  version: string,
  since: [@default ""] string,
  clock: string,
  is_fresh_instance: bool,
}
and expression =
  | [@name "match"] Match(string)
  | [@name "allof"] Allof2(expression, expression)
  | [@name "allof"] Allof3(expression, expression, expression)
and command =
  | [@name "query"] Query(string, query_schema)
  | [@name "watch-project"] WatchProject(string)
  | [@name "subscribe"] Subscribe(string, string, subscribe_schema)
and response =
  | QueryResponse(query_response)
  | WatchProjectResponse(watch_project_response)
  | SubscribeResponse(subscribe_response)
and subscribe_schema = {
  expression,
  path: list(string),
  fields: list(string),
}
and query_schema = {
  case_sensitive: bool,
  path: list(string),
  fields: list(string),
  expression,
};

let result = a =>
  switch (a) {
  | Result.Ok(b) => b
  };

let sock_addr_of_json = json_str => {
  let sockname =
    result(sockname_response_of_yojson(Yojson.Safe.from_string(json_str))).
      sockname;
  Unix.ADDR_UNIX(sockname);
};

let socket_of_addr = sockaddr => {
  let socket = Lwt_unix.socket(Unix.PF_UNIX, Unix.SOCK_STREAM, 0);
  Lwt_unix.connect(socket, sockaddr);
  socket;
};

let subscription_message_of_str = line =>
  result(subscription_message_of_yojson(Yojson.Safe.from_string(line)));

let print_files = (files: list(string)) =>
  List.iter(
    file => {
      Lwt_io.printf("File: %s\n", file);
      ();
    },
    files,
  );

let rec handle_changed_files = (stream, handler) : Lwt.t(_) => {
  let%lwt line = Lwt_stream.last_new(stream);
  let res = subscription_message_of_str(line);
  let changed = List.map(file => Filename.concat(res.root, file), res.files);
  handler(changed);
  handle_changed_files(stream, handler);
};

let read_line = ch => {
  let%lwt line = Lwt_io.read_line(ch);
  let json = Yojson.Safe.from_string(line);
  Lwt.return(json);
};

let command_to_string = (cmd: command) =>
  Yojson.Safe.to_string(command_to_yojson(cmd));

let send_cmd = ({input, output}, cmd: command) => {
  Lwt_io.write_line(output, command_to_string(cmd));
  let%lwt line = read_line(input);
  Lwt.return(line);
};

let send_watch_project = (io, dir) => {
  let%lwt res = send_cmd(io, WatchProject(dir));
  Lwt.return(result(watch_project_response_of_yojson(res)));
};

let send_subscribe = (io, root, name, args) => {
  let%lwt res = send_cmd(io, Subscribe(root, name, args));
  Lwt.return(result(subscribe_response_of_yojson(res)));
};

let send_query = (io, root, args) => {
  let%lwt res = send_cmd(io, Query(root, args));
  Lwt.return(result(query_response_of_yojson(res)));
};

let get_io = () => {
  let%lwt addr =
    Lwt_process.pread(("", [|"watchman", "get-sockname", "--no-pretty"|]));
  let sockaddr = sock_addr_of_json(addr);
  let socket = socket_of_addr(sockaddr);
  let input = Lwt_io.of_fd(~mode=Lwt_io.input, socket);
  let output = Lwt_io.of_fd(~mode=Lwt_io.output, socket);
  Lwt.return({input, output});
};

let startup_watchman = dir => {
  let%lwt io = get_io();
  let%lwt watch_res = send_watch_project(io, dir);
  Lwt.return((io, watch_res));
};

let query = dir => {
  let%lwt (io, watch_res) = startup_watchman(dir);
  let%lwt query_res =
    send_query(
      io,
      watch_res.watch,
      {
        path: [watch_res.relative_path],
        case_sensitive: false,
        expression: Match("*.ess"),
        fields: ["name"],
      },
    );
  print_files(query_res.files);
  Lwt.return();
};

let watch = dir => {
  let%lwt (io, watch_res) = startup_watchman(dir);
  let%lwt sub_res =
    send_subscribe(
      io,
      watch_res.watch,
      "mysubscriptionname",
      {
        path: [watch_res.relative_path],
        expression: Match("*.ess"),
        fields: ["name"],
      },
    );

  Lwt_io.printf("clock: %s\n", sub_res.clock);
  let stream = Lwt_io.read_lines(io.input);
  let avail = Lwt_stream.get_available(stream);
  List.iter(
    line => {
      let res = subscription_message_of_str(line);
      let files =
        List.map(file => Filename.concat(res.root, file), res.files);
      print_endline("starting files");
      print_files(files);
    },
    avail,
  );
  handle_changed_files(stream, print_files);
};

let run_watch = () => Lwt_main.run(watch(Sys.getcwd()));
let run_query = () => Lwt_main.run(query(Sys.getcwd()));
