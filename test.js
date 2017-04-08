/*
// こうすれば、defineを複数実行可能になるわけか・・・
if(process.argv[2] == "def") {
  console.log("__HOGEHOGE__ __HAGEHAGE__");
}
else {
  console.log(" __HELLO__");
}
var os = require("os");

//console.log(os);
var exec = require("child_process").execSync;

if("" + exec("pkg-config --exists libavcodec")) {
  console.log("libavcodecあった");
}
// まぁ、これでいいや。
// cflags-only-Iからincludeパスがほしい。
// libsの内容がそのまま欲しいの２点か・・・
if(exec("pkg-config --exists libavcodec && echo yes || echo no").toString().trim() == "yes") {
  console.log("ある");
}
else {
  console.log("ない");
}

//*/

// これでプロジェクト上にある、設定が読み込めるか・・・
// でもこれだと、A -> B -> ttLibJsGyp2という継承状態になってると、Bの設定が有効にならないな・・・
// ま、いいか。使いたいプロジェクトで設定する感じでいいだろう。
var fs = require("fs");
try {
var obj = JSON.parse(fs.readFileSync('../../tak_setting.json'));
fs.writeFile("memo.txt", JSON.stringify(obj), () => {
});
}
catch(e) {
try {
var obj = JSON.parse(fs.readFileSync('tak_setting.json'));
fs.writeFile("memo.txt", JSON.stringify(obj), () => {
});
}
catch(e) {

}

}
