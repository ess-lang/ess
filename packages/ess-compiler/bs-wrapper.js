require('./bs.js');

module.exports = {
  compile: (str) => JSON.parse(global.ocaml.compile(str)).js_code
};
