var tt = require("..");
var loopback = new tt.Loopback("Japanese");

var writer = new tt.writer.MkvWriter(1000, "pcmS16");
//var encoder = new tt.encoder.MSFaacEncoder(48000, 2, 96000);

var writableStream = require("fs").createWriteStream("output2.mkv");

setInterval(() => {
  loopback.queryFrame((err, frame) => {
    frame.id = 1;
    return writer.writeFrame(frame, (err, data) => {
      writableStream.write(data);
      return true;
    });
  });
}, 500);