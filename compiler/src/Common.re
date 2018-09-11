module StrHash =
  Hashtbl.Make({
    type t = string;
    let hash = Hashtbl.hash;
    let equal = (==);
  });

module StrSet =
  Set.Make({
    type t = string;
    let compare = Pervasives.compare;
  });
