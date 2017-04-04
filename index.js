var ttLibJsGyp = require("bindings")("ttLibJsGyp");

module.exports = {
  "Frame": ttLibJsGyp.Frame,
  "reader": {
    "FlvReader": function() {
      return ttLibJsGyp.Reader("flv");
    },
    "MkvReader": function() {
      return ttLibJsGyp.Reader("mkv");
    },
    "WebmReader": function() {
      return ttLibJsGyp.Reader("webm");
    },
    "Mp4Reader": function() {
      return ttLibJsGyp.Reader("mp4");
    },
    "MpegtsReader": function() {
      return ttLibJsGyp.Reader("mpegts");
    }
  },
  "writer": {
    "FlvWriter": function(videoCodec, audioCodec) {
      return ttLibJsGyp.Writer("flv", videoCodec, audioCodec);
    },
    "MkvWriter": function(unitDuration, ...codecs) {
      var args = [].concat(codecs);
      args.unshift(unitDuration);
      args.unshift("mkv");
      return ttLibJsGyp.Writer.apply(null, args);
    },
    "WebmWriter": function(unitDuration, ...codecs) {
      var args = [].concat(codecs);
      args.unshift(unitDuration);
      args.unshift("webm");
      return ttLibJsGyp.Writer.apply(null, args);
    },
    "Mp4Writer": function(unitDuration, ...codecs) {
      var args = [].concat(codecs);
      args.unshift(unitDuration);
      args.unshift("mp4");
      return ttLibJsGyp.Writer.apply(null, args);
    },
    "MpegtsWriter": function(unitDuration, ...codecs) {
      var args = [].concat(codecs);
      args.unshift(unitDuration);
      args.unshift("mpegts");
      return ttLibJsGyp.Writer.apply(null, args);
    }
  }
};
