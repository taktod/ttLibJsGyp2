var fs = require("fs");
var exec = require("child_process").execSync;
// 動作ターゲット defs:定義 libs:ライブラリ includes:ヘッダファイル
var target = process.argv[2];
// 設定ファイルを読み込んでどのようなコンパイルにするかコントロールする。
/*
{
  "target": "bsd" | "lgpl" | "gpl", // 動作モード指定
  "disable": ["jpeg", "faac" ...], // 除外するライブラリ指定
  "searchPath": ["c:¥¥hogehoge¥¥"] // 検索に追加するパス
}
 */
var setting = loadSetting();

/*
チェックすべきライブラリ
file socketについては、常に有効(emscriptenとかの別のコンパイルはしないので、考慮しない。)
opencv openalは放置
jpeg       any                jpeglib.h
openh264   any                pkgconfig
x264       gpl                pkgconfig
x265       gpl                pkgconfig
theora     any                pkgconfig
daala      any                pkgconfig
mp3lame    lgpl(enc) gpl(dec) lame/lame.h libmp3lame.a
faac       lgpl               faac.h
speexdsp   any                pkgconfig
speex      any                pkgconfig
opus       any                pkgconfig
faad       gpl                無視
vorbis     any                pkgconfig
avcodec    lgpl               pkgconfig
apple      darwin only        darwin or not
webrtc     any?               無視(electronで攻めるので、必要ないはず)
soundtouch lgpl               pkgconfig
fdkaac     lgpl               
こんなところか・・・
 */

setupAvcodec();
setupApple();
setupFaac();
setupFdkaac();
setupJpeg(); 
setupMp3lame();
setupOpenh264();
setupOpus();
setupTheora();
setupSoundtouch();
setupSpeex();
setupSpeexdsp();
setupVorbis();
setupX264();
setupX265();
//setupDaala();
//setupFaad();

function setupAvcodec() {
  if(setting["targetValue"] < 1) {
    // lgpl以上じゃないと動作させない
    return;
  }
  if(setting["disable"].indexOf("avcodec") != -1) {
    // avcodecは無効になってる。
    return;
  }
  // とりあえず書くだけ書いてみよう
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists libavcodec libavutil && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_AVCODEC__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs libavcodec libavutil").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I libavcodec libavutil | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  default:
    break;
  }
}

function setupApple() {

}

function setupFaac() {

}

function setupFdkaac() {

}

function setupJpeg() {

}

function setupMp3lame() {

}

function setupOpenh264() {
  if(setting["disable"].indexOf("openh264") != -1) {
    // openh264は無効になってる
    return;
  }
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    if(exec("pkg-config --exists openh264 && echo yes || echo no").toString().trim() == "yes") {
      // pkg-configで存在してる。
      switch(target) {
      case "defs":
        console.log("__ENABLE_OPENH264__");
        break;
      case "libs":
        console.log(exec("pkg-config --libs openh264").toString().trim());
        break;
      case "includes":
        console.log(exec("pkg-config --cflags-only-I openh264 | sed -e 's/\-I//g'").toString().trim());
        break;
      }
    }
    break;
  case "windows":
  default:
    break;
  }
}

function setupOpus() {

}

function setupTheora() {

}

function setupSoundtouch() {

}

function setupSpeex() {

}

function setupSpeexdsp() {

}

function setupVorbis() {

}

function setupX264() {

}

function setupX265() {

}

function loadSetting() {
  var os = require("os");
  var setting = (function() {
    try {
      return JSON.parse(fs.readFileSync('../../takSetting.json'));
    }
    catch(e) {
      try {
        return JSON.parse(fs.readFileSync('takSetting.json'));
      }
      catch(e) {
        return {};
      }
    }
  })();
  if(!setting["target"]) {
    setting["target"] = "bsd";
  }
  switch(setting["target"]) {
  case "gpl":
    setting["targetValue"] = 2;
    break;
  case "lgpl":
    setting["targetValue"] = 1;
    break;
  default:
    setting["targetValue"] = 0;
    break;
  }
  if(!setting["disable"]) {
    setting["disable"] = [];
  }
  if(!setting["searchPath"]) {
    setting["searchPath"] = [];
  }
  setting["os"] = os.type().toString().toLowerCase();
  switch(setting["os"]) {
  case "darwin":
  case "linux":
    setting["searchPath"].push("/usr/local/include");
    setting["searchPath"].push("/usr/local/lib");
    break;
  case "windows": // これはwindows_ntとかになるかも
    break;
  default:
    break;
  }
  return setting;
}







/*
switch(os.type().toString().toLowerCase()) {
case "darwin":
  checkDarwin(target);
  break;
case "linux":
  checkLinux(target);
  break;
case "windows":
  checkWindow(target);
  break;
default:
  return;
}

function checkDarwin(type) {
  // 必要なライブラリをリストアップしておく
  exec("");
}

function checkLinux(type) {

}

function checkWindow(type) {

}

*/