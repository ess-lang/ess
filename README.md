# ESS: element style sheets

> Note: this project is still a work in progress and many features are incomplete, missing, or broken.

ESS is a language that compiles to typed, stateless JSX components and an associated static CSS bundle.

ESS is designed to be a *replacement* for CSS-in-JS while providing an abstraction similar to [styled-components](https://github.com/styled-components/styled-components) et al.

## Motivation

### Performance overhead of CSS-in-JS

Abstraction in JS comes at a cost ([prepack](https://github.com/facebook/prepack) notwithstanding), so CSS-in-JS can incur a significant performance penalty. Composition, a primary concern for UI styling, necessitates dead code and overhead in JS. To make matters worse, the JS tooling ecosystem doesn't provide an great way to address this overhead and solutions tend to be hacky or introduce unnatural limitations on JS.

### Subpar type safety of CSS-in-JS

Object literals can be typed, but CSS involves lots of "stringly" typed values so even when using TypeScript or Flow, there isn't a great typing experience.

### Awkward semantics and syntax of CSS-in-JS

Authoring styling code in JS tends to be rather verbose.

## Features

### Static types and pattern matching

Pattern matching makes styling logic incredibly elegant.

Additionally, the JSX components outputted by the ESS are typed.

### No cascade, selectors, or specificity

ESS has no concept of selectors or specificity whatsoever. Instead, element styling is expressed as a pure function.

### No inheritance by default

Elements will always be styled the same regardless of their location in DOM or existing CSS. In other words, element styles do not depend on their parent/ancestors. Instead, composition of styles must be explicit within ESS.

### Optimized, static CSS output

There are no truly "dynamic" values in ESS, therefore a static CSS bundle for all possible styles can be produced. This also means RTL and vendor prefixing can be performed at build-time. Furthermore, ESS produces an optimized, atomic CSS stylesheet with no caveats.

## Trade-offs
See [TRADEOFFS.md](https://github.com/rtsao/ess/blob/master/TRADEOFFS.md)

## Other motivation

**Teaching myself**

I am using ESS as a project to gain more experience with:
- ReasonML and OCaml
- [Language server protocol](https://github.com/Microsoft/language-server-protocol) (LSP)
- Compiler design and implementation

**Teaching others**

I hope ESS can eventually serve as a useful reference implementation of a production-grade programming language built with modern tools, but for an extremely simple programming language so it is approachable.

To that end, my goal is for ESS to have all the expected bells and whistles of a modern language including:
- IDE integration via LSP
- Automatic AST-based code formatting like [prettier](https://github.com/prettier/prettier) or refmt
- Fast, native build performance
- Web-based REPL-like playground
- Human-friendly compiler errors
- Watch-compile mode with incremental compilation
