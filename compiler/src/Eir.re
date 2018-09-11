/* ESS Intermediate Representation */
module SS = Set.Make(String);
module SM = Map.Make(String);

type param_type =
  | StringEnumType(SS.t)
  | NumberType
  | BooleanType
and element_body = {interface: SM.t(param_type)}
and node =
  | Element(element_body)
and attribute =
  | BorderAttribute
  | PaddingAttribute
  | MarginAttribute
  | BackgroundAttribute
and color =
  | RGBA(int, int, int, int)
and length =
  | Px(float)
and margin = {
  top: margin_value,
  right: margin_value,
  bottom: margin_value,
  left: margin_value,
}
and margin_value =
  | Margin(length)
and border = {
  top: border_value,
  right: border_value,
  bottom: border_value,
  left: border_value,
}
and border_value =
  | Border(border_pattern, length, color)
  /* https://www.w3.org/TR/CSS2/tables.html#border-conflict-resolution */
  | NoBorder
  | HiddenBorder
and border_pattern =
  | DashedBorder
  | SolidBorder;
