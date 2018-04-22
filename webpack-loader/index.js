const fs = require('fs');
const path = require('path');
const essCompiler = require('@ess/compiler');

module.exports = function essLoader(content) {
  if (path.extname(this.resourcePath) !== '.ess') {
    return;
  }
  const {js_code, flow_def, css} =  essCompiler.compile_js(content);
  return js_code;
};
