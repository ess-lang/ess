const resolvePkg = require('resolve-pkg');
const fs = require('fs');
const path = require('path');

const bsPath = resolvePkg('bsb-native', {cwd: __dirname});

const files = ['cmi', 'cmj', 'cmt', 'cmti', 'ml', 'mli'].map(
  ext => `pervasives.${ext}`,
);

const dest = path.resolve(__dirname, '../lib/bs/native/ocaml');

fs.mkdir(dest, err => {
  if (err) throw err;
  files.forEach(file => {
    fs.copyFile(
      path.join(bsPath, 'lib/ocaml', file),
      path.join(dest, file),
      err => {
        if (err) throw err;
      },
    );
  });
});
