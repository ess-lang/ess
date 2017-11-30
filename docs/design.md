
## Basic example

```elm
Foo [
  @size "small" | "medium" | "large"

  color red
  border (x: (color: red, width: 2))
  shadow @size {
    "small" => (offset: 15, size: 15, color: red)
    "medium" => (offset: 25, size: 25, color: red)
    "large" => (offset: 35, size: 35, color: red)
  }
]
```

## Composition

```elm
Foo [
  color red
]

Bar [
  include Foo

  background green
]
```

## Pattern matching

#### Value expressions
```elm
Foo [
  @prop boolean

  color @prop {
    true => red
    false => blue
  }
]
```

#### Block expressions
```elm
Foo [
  @prop boolean

  @prop {
    true => [
      color white
      background red
    ]
    false => background blue
  }
]
```

### Exhaustiveness
```elm
Foo [
  @a bool
  @b number
  @c "x" | "y" | "z"

  color @a {
    true => red
    false => blue
  }
  color @b {
    1..10  => red
    10..50 => blue
    _ => green
  }
  color @c {
    "x" => red
    "y" | "z" => blue
  }
]
```

### Tuple matching

```elm
Hello [
  @foo boolean
  @bar boolean

  (@foo, @bar) {
    (true, true) => color red
    (false, false) => color blue
    (_, _) => color green
  }
]
```
