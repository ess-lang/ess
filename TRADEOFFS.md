## Creating a new language instead of extending CSS
### Upsides
- Significant improvements over CSS syntax and semantics (type safety, pattern matching, etc.)
- No uncanny valley
### Downsides
- Users have to install yet another tool
- Tooling has to be built and maintained
- Docs have to be written and maintained
- Not already well-established, will be something new to learn
- Second-class citizenship in Chrome DevTools. Extra tooling, such as custom extension may be necessary to fill gaps. Clean mapping for source maps might be a challenge

## Multiple app runtime targets
### Upsides
- Interoperability and portability
### Downsides
- Targeting multiple app runtimes means adopting latest and greatest of platforms may be slower
- May be a lot of work to maintain multiple compiler backends, unique optimizations may be required
- Language semantics and abstractions may not be tenable if targets platforms diverge or change
