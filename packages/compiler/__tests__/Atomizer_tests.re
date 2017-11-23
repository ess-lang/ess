open Jest;

open Atomizer;

open Styles;

describe(
  "Atomizer.string_of_color",
  ExpectJs.(
    () =>
      test(
        "non-integer float",
        () =>
          expect(string_of_color(RGBA(0, 1, 100, 255)))
          |> toBe("rgba(0,1,100,255)")
      )
  )
);

describe(
  "Atomizer.css_string_of_float",
  ExpectJs.(
    () => {
      test(
        "non-integer float",
        () => expect(css_string_of_float(1.2)) |> toBe("1.2")
      );
      test(
        "integer float",
        () => expect(css_string_of_float(1.0)) |> toBe("1")
      );
      test(
        "integer float with more precision",
        () => expect(css_string_of_float(1.000)) |> toBe("1")
      );
      test("zero", () => expect(css_string_of_float(0.0)) |> toBe("0"))
    }
  )
);

describe(
  "Atomizer.string_of_length",
  ExpectJs.(
    () => {
      test("px", () => expect(string_of_length(Px(2.0))) |> toBe("2px"));
      test("em", () => expect(string_of_length(Em(5.5))) |> toBe("5.5em"))
    }
  )
);

describe(
  "Atomizer.string_of_style",
  ExpectJs.(
    () => {
      test(
        "color",
        () =>
          expect(string_of_style(Color(RGBA(1, 2, 3, 4))))
          |> toBe("(color: rgba(1,2,3,4))")
      );
      test(
        "border",
        () =>
          expect(
            string_of_style(Border(Top, Px(2.0), Solid, RGBA(1, 4, 3, 2)))
          )
          |> toBe("(border: top 2px solid rgba(1,4,3,2))")
      )
    }
  )
);
