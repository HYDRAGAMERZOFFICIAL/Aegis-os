const fs = require('fs');
const content = fs.readFileSync('package.json', 'utf8');
const cleaned = content.replace(/`r`n\s*/g, '\n');
const pkg = JSON.parse(cleaned);
pkg.scripts.build = "node -e \"const fs=require('fs'); fs.cpSync('simulator/web','public',{recursive:true});\"";
fs.writeFileSync('package.json', JSON.stringify(pkg, null, 2) + '\n', 'utf8');
console.log('package.json written cleanly.');

