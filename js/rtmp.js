var tt = require("../");

var nc = new tt.rtmp.NetConnection();
var ns = null;
nc.addEventListener("onStatusEvent", (event) => {
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
});
nc.connect("rtmp://rtmpTestServer.com/live");
