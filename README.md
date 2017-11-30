# Element style sheets

> Note: this project is highly experimental and not yet ready for production usage. Expect many breaking changes and instability for the time being. It is a work in progress and many features may be incomplete or missing. The current level of quality is somewhere between proof-of-concept and prototype and there are few automated tests.

ESS is a language for styling UI components

### Goals
- Elegant syntax and semantics
    - Intuitiveness for users familiar with CSS-in-JS and languages such as Reason/Rust
    - Ability to express styling in a natural, declarative, DRY manner
- Output with best possible performance
    - Fully build-time style processing including vendor prefixing and RTL transformation
    - Zero-cost algebraic composition/transformation of styles
    - Ability to rely on the compiler to make optimizations rather than working around underlying tools
- Robustness and type safety
- Good developer tooling
    - Helpful type checker and compiler warnings
    - Integration with tools such as Flow
    - Language Server Protocol support for editor integration (autocompletion, syntax highlighting, etc.)
- Support for dynamic styles when performance/robustness/safety isn't impacted
- Multiple app runtime targets
- No lock-in. Easily "eject" by compiling down to standard primitives (CSS, etc). Viola, no more ESS.

### Non-goals
- Feature parity with entirety of CSS
- Preservation of CSS syntax and semantics
- Turing completeness

### Unknowns
- Layout
  - Mirroring Flexbox/Yoga semantics is safest option, but higher-level layout semantics may be nicer.
  - Enforcing separation of styling and layout à la https://github.com/mdgriffith/style-elements may be desirable, but this a big change from CSS.

### Trade-offs
See [TRADEOFFS.md](https://github.com/rtsao/ess/blob/master/TRADEOFFS.md)

### Language design
See [docs/design.md](https://github.com/rtsao/ess/blob/master/docs/design.md)
