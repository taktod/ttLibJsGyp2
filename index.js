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
    AudioConverterEncoder: function(type, sampleRate, channelNum, bitrate) {
      return ttLibJsGyp.Encoder.apply(null, ["audioConverter", {type: type, sampleRate: sampleRate, channelNum: channelNum, bitrate: bitrate}]);
    },
    FaacEncoder: function(type, sampleRate, channelNum, bitrate) {
      return ttLibJsGyp.Encoder.apply(null, ["faac", {type: type, sampleRate: sampleRate, channelNum: channelNum, bitrate: bitrate}]);
    },
    Mp3lameEncoder: function(sampleRate, channelNum, quality) {
      return ttLibJsGyp.Encoder.apply(null, ["mp3lame", {sampleRate: sampleRate, channelNum: channelNum, quality:quality}]);
    },
    JpegEncoder: function(width, height, quality) {
      return ttLibJsGyp.Encoder.apply(null, ["jpeg", {width: width, height: height, quality: quality}]);
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
    VtCompressSessionEncoder: function(type, width, height, fps=15, bitrate=320000, isBaseline=true) {
      return ttLibJsGyp.Encoder.apply(null, ["vtCompressSession", {type: type, width: width, height: height, fps: fps, bitrate: bitrate, isBaseline: isBaseline}]);
    },
    X264Encoder: function(width, height, preset="", tune="", profile="", param={}) {
      if(!profile || profile == "") {
        profile = "baseline";
      }
      return ttLibJsGyp.Encoder.apply(null, ["x264", {width: width, height: height, preset: preset, tune: tune, profile: profile, param: param}]);
    },
    X265Encoder: function(width, height, preset="", tune="", profile="", param={}) {
      if(!profile || profile == "") {
        profile = "main";
      }
      return ttLibJsGyp.Encoder.apply(null, ["x265", {width: width, height: height, preset: preset, tune: tune, profile: profile, param: param}]);
    }
  },
  resampler: {
    AudioResampler: function(type, subType, channelNum=0) {
      return ttLibJsGyp.Resampler.apply(null, ["audio", {type: type, subType: subType, channelNum: channelNum}]);
    },
    ImageResampler: function(type, subType) {
      return ttLibJsGyp.Resampler.apply(null, ["image", {type: type, subType: subType}]);
    },
/*  // imageResizerは放置しておく。とりあえずelectronで使う予定だったら、webGLでresize処理は済むし・・・
    ImageResizer: function(){
    },*/
    SoundtouchResampler: function(sampleRate, channelNum) {
      return ttLibJsGyp.Resampler.apply(null, ["soundtouch", {sampleRate: sampleRate, channelNum: channelNum}]);
    },
    SpeexdspResampler: function(channelNum, inSampleRate, outSampleRate, quality) {
      return ttLibJsGyp.Resampler.apply(null, ["speexdsp", {channelNum: channelNum, inSampleRate: inSampleRate, outSampleRate: outSampleRate, quality: quality}]);
    }
  },
  rtmp: {
    NetConnection: function() {
      return ttLibJsGyp.NetConnection();
    },
    NetStream: function(nc) {
      return ttLibJsGyp.NetStream(nc);
    }
  }
};
