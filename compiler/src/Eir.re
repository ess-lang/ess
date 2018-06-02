/* ESS Intermediate Representation */

type attribute =
  | BorderAttribute
  | PaddingAttribute
  | MarginAttribute
  | BackgroundAttribute;

type color =
  | RGBA(int, int, int, int)
and length =
  | Px(float);

type margin = {
  top: margin_value,
  right: margin_value,
  bottom: margin_value,
  left: margin_value
}
and margin_value =
  | Margin(length);

type border = {
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
