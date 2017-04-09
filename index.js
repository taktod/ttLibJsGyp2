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
  },
  encoder: {
    FaacEncoder: function(type, sampleRate, channelNum, bitrate) {
      return ttLibJsGyp.Encoder.apply(null, ["faac", {type: type, sampleRate: sampleRate, channelNum: channelNum, bitrate: bitrate}]);
    },
    Mp3lameEncoder: function(sampleRate, channelNum, quality) {
      return ttLibJsGyp.Encoder.apply(null, ["mp3lame", {sampleRate: sampleRate, channelNum: channelNum, quality:quality}]);
    },
    Openh264Encoder: function(width, height, param, spatialParamArray) {
      return ttLibJsGyp.Encoder.apply(null, ["openh264", {width: width, height: height, param: param, spatialParamArray: spatialParamArray}]);
    },
    OpusEncoder: function(sampleRate, channelNum, unitSampleNum) {
      return ttLibJsGyp.Encoder.apply(null, ["opus", {sampleRate: sampleRate, channelNum: channelNum, unitSampleNum: unitSampleNum}]);
    },
    TheoraEncoder: function(width, height, quality, bitrate, keyFrameInterval) {
      return ttLibJsGyp.Encoder.apply(null, ["theora", {width: width, height: height, quality: quality, bitrate: bitrate, keyFrameInterval: keyFrameInterval}]);
    },
    X264Encoder: function(width, height, preset="", tune="", profile="", param={}) {
      if(!profile || profile == "") {
        profile = "baseline";
      }
      return ttLibJsGyp.Encoder.apply(null, ["x264", {width: width, height: height, preset: preset, tune: tune, profile: profile, param: param}]);
    }
  },
  resampler: {
    AudioResampler: function(type, subType, channelNum=0) {
      return ttLibJsGyp.Resampler.apply(null, ["audio", {type: type, subType: subType, channelNum: channelNum}]);
    },
    SpeexdspResampler: function(channelNum, inSampleRate, outSampleRate, quality) {
      return ttLibJsGyp.Resampler.apply(null, ["speexdsp", {channelNum: channelNum, inSampleRate: inSampleRate, outSampleRate: outSampleRate, quality: quality}]);
    }
  }
};
