var tt = require("../");
var fs = require("fs");

var nc = new tt.rtmp.NetConnection();
var ns = null;
// 視聴動作の場合
/*nc.addEventListener("onStatusEvent", (event) => {
  console.log(event);
  switch(event.info.code) {
  case "NetConnection.Connect.Success":
    ns = new tt.rtmp.NetStream(nc);
    ns.setBufferLength(2);
    ns.addEventListener("onStatusEvent", (event) => {
      console.log(event);
    });
    ns.setFrameListener((err, frame) => {
      if(frame.type == "aac") {
        console.log(frame.type + " " + frame.pts);
      }
    });
    ns.play("test");
    break;
  default:
    break;
  }
});*/

// 配信動作の場合
var reader = new tt.reader.FlvReader();
var readableStream = fs.createReadStream(process.env.HOME + "/tools/data/source/test.h264.aac.flv");

var frames = [];
var startTime = 0;
nc.addEventListener("onStatusEvent", (event) => {
  switch(event.info.code) {
  case "NetConnection.Connect.Success":
    ns = new tt.rtmp.NetStream(nc);
    ns.publish("test");
    ns.addEventListener("onStatusEvent", (event) => {
      if(event.info.code == "NetStream.Publish.Start") {
        readableStream.on("data", (data) => {
          reader.readFrame(data, (err, frame) => {
            // 本来なら、frameをためて、タイミングをあわせて送信するようにしなければならない。
            // 前のttLibJsGypだったら、frameデータが完全コピーなので、cacheしたりできるけど、
            // 今回の動作だと内部でttLibC_Frameのオブジェクトが読み込みを実施するたびに更新されるので
            // できなくなってる
            ns.queueFrame(frame);
            return true;
          });
        });
      }
    });
    break;
  default:
    break;
  }
});

nc.connect("rtmp://rtmpTestServer.com/live");
