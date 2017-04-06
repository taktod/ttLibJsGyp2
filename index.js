var ttLibJsGyp = require("bindings")("ttLibJsGyp");

module.exports = {
  Frame: ttLibJsGyp.Frame,
  reader: {
    FlvReader: function() {
      return ttLibJsGyp.Reader("flv");
    },
    MkvReader: function() {
      return ttLibJsGyp.Reader("mkv");
    },
    WebmReader: function() {
      return ttLibJsGyp.Reader("webm");
    },
    Mp4Reader: function() {
      return ttLibJsGyp.Reader("mp4");
    },
    MpegtsReader: function() {
      return ttLibJsGyp.Reader("mpegts");
    }
  },
  writer: {
    FlvWriter: function(videoCodec, audioCodec) {
      return ttLibJsGyp.Writer("flv", videoCodec, audioCodec);
    },
    MkvWriter: function(unitDuration, ...codecs) {
      return ttLibJsGyp.Writer.apply(null, ["mkv", unitDuration, codecs]);
    },
    WebmWriter: function(unitDuration, ...codecs) {
      return ttLibJsGyp.Writer.apply(null, ["webm", unitDuration, codecs]);
    },
    Mp4Writer: function(unitDuration, ...codecs) {
      return ttLibJsGyp.Writer.apply(null, ["mp4", unitDuration, codecs]);
    },
    MpegtsWriter: function(unitDuration, ...codecs) {
      return ttLibJsGyp.Writer.apply(null, ["mpegts", unitDuration, codecs]);
    }
  },
  decoder: {
    AvcodecVideoDecoder: function(type, width, height) {
      return ttLibJsGyp.Decoder.apply(null, ["avcodec", {type: type, width: width, height: height}]);
    },
    AvcodecAudioDecoder: function(type, sampleRate, channelNum) {
      return ttLibJsGyp.Decoder.apply(null, ["avcodec", {type: type, sampleRate: sampleRate, channelNum: channelNum}]);
    }
  }
};
