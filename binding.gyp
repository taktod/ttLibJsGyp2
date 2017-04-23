﻿{
  "targets": [
    {
      "defines": [
        "<!@(node libcheck.js defs)",
        "__ENABLE_SOCKET__"
      ],
      "type": "static_library",
      "target_name": "ttLibC",
      "sources": [
        "ttLibC/ttLibC/ttLibC.c",
        "ttLibC/ttLibC/allocator.c",
        "ttLibC/ttLibC/container/container.c",
        "ttLibC/ttLibC/container/misc2.c",
        "ttLibC/ttLibC/container/flv/flvReader.c",
        "ttLibC/ttLibC/container/flv/flvTag.c",
        "ttLibC/ttLibC/container/flv/flvWriter.c",
        "ttLibC/ttLibC/container/flv/type/audioTag.c",
        "ttLibC/ttLibC/container/flv/type/headerTag.c",
        "ttLibC/ttLibC/container/flv/type/metaTag.c",
        "ttLibC/ttLibC/container/flv/type/videoTag.c",
        "ttLibC/ttLibC/container/mkv/mkvReader.c",
        "ttLibC/ttLibC/container/mkv/mkvTag.c",
        "ttLibC/ttLibC/container/mkv/mkvWriter.c",
        "ttLibC/ttLibC/container/mkv/type/simpleBlock.c",
        "ttLibC/ttLibC/container/mp3/mp3Frame.c",
        "ttLibC/ttLibC/container/mp3/mp3Reader.c",
        "ttLibC/ttLibC/container/mp3/mp3Writer.c",
        "ttLibC/ttLibC/container/mp4/mp4Atom.c",
        "ttLibC/ttLibC/container/mp4/mp4Reader.c",
        "ttLibC/ttLibC/container/mp4/mp4Writer.c",
        "ttLibC/ttLibC/container/mp4/type/ctts.c",
        "ttLibC/ttLibC/container/mp4/type/elst.c",
        "ttLibC/ttLibC/container/mp4/type/stco.c",
        "ttLibC/ttLibC/container/mp4/type/stsc.c",
        "ttLibC/ttLibC/container/mp4/type/stsz.c",
        "ttLibC/ttLibC/container/mp4/type/stts.c",
        "ttLibC/ttLibC/container/mp4/type/trun.c",
        "ttLibC/ttLibC/container/mpegts2/mpegtsPacket.c",
        "ttLibC/ttLibC/container/mpegts2/mpegtsReader.c",
        "ttLibC/ttLibC/container/mpegts2/mpegtsWriter.c",
        "ttLibC/ttLibC/container/mpegts2/type/pat.c",
        "ttLibC/ttLibC/container/mpegts2/type/pes.c",
        "ttLibC/ttLibC/container/mpegts2/type/pmt.c",
        "ttLibC/ttLibC/container/mpegts2/type/sdt.c",
        "ttLibC/ttLibC/decoder/avcodecDecoder.c",
        "ttLibC/ttLibC/encoder/audioConverterEncoder.c",
        "ttLibC/ttLibC/encoder/faacEncoder.c",
        "ttLibC/ttLibC/encoder/jpegEncoder.c",
        "ttLibC/ttLibC/encoder/mp3lameEncoder.c",
        "ttLibC/ttLibC/encoder/openh264Encoder.cpp",
        "ttLibC/ttLibC/encoder/opusEncoder.c",
        "ttLibC/ttLibC/encoder/theoraEncoder.c",
        "ttLibC/ttLibC/encoder/vtCompressSessionEncoder.c",
        "ttLibC/ttLibC/encoder/x264Encoder.c",
        "ttLibC/ttLibC/encoder/x265Encoder.c",
        "ttLibC/ttLibC/frame/frame.c",
        "ttLibC/ttLibC/frame/audio/aac.c",
        "ttLibC/ttLibC/frame/audio/adpcmImaWav.c",
        "ttLibC/ttLibC/frame/audio/audio.c",
        "ttLibC/ttLibC/frame/audio/mp3.c",
        "ttLibC/ttLibC/frame/audio/nellymoser.c",
        "ttLibC/ttLibC/frame/audio/opus.c",
        "ttLibC/ttLibC/frame/audio/pcmAlaw.c",
        "ttLibC/ttLibC/frame/audio/pcmf32.c",
        "ttLibC/ttLibC/frame/audio/pcmMulaw.c",
        "ttLibC/ttLibC/frame/audio/pcms16.c",
        "ttLibC/ttLibC/frame/audio/speex.c",
        "ttLibC/ttLibC/frame/audio/vorbis.c",
        "ttLibC/ttLibC/frame/video/bgr.c",
        "ttLibC/ttLibC/frame/video/flv1.c",
        "ttLibC/ttLibC/frame/video/h264.c",
        "ttLibC/ttLibC/frame/video/h265.c",
        "ttLibC/ttLibC/frame/video/jpeg.c",
        "ttLibC/ttLibC/frame/video/theora.c",
        "ttLibC/ttLibC/frame/video/video.c",
        "ttLibC/ttLibC/frame/video/vp6.c",
        "ttLibC/ttLibC/frame/video/vp8.c",
        "ttLibC/ttLibC/frame/video/vp9.c",
        "ttLibC/ttLibC/frame/video/wmv1.c",
        "ttLibC/ttLibC/frame/video/wmv2.c",
        "ttLibC/ttLibC/frame/video/yuv420.c",
        "ttLibC/ttLibC/net/tetty/context.c",
        "ttLibC/ttLibC/net/tetty/promise.c",
        "ttLibC/ttLibC/net/client/rtmp/data/clientObject.c",
        "ttLibC/ttLibC/net/client/rtmp/header/rtmpHeader.c",
        "ttLibC/ttLibC/net/client/rtmp/message/acknowledgement.c",
        "ttLibC/ttLibC/net/client/rtmp/message/aggregateMessage.c",
        "ttLibC/ttLibC/net/client/rtmp/message/amf0Command.c",
        "ttLibC/ttLibC/net/client/rtmp/message/amf0DataMessage.c",
        "ttLibC/ttLibC/net/client/rtmp/message/audioMessage.c",
        "ttLibC/ttLibC/net/client/rtmp/message/rtmpMessage.c",
        "ttLibC/ttLibC/net/client/rtmp/message/setChunkSize.c",
        "ttLibC/ttLibC/net/client/rtmp/message/setPeerBandwidth.c",
        "ttLibC/ttLibC/net/client/rtmp/message/userControlMessage.c",
        "ttLibC/ttLibC/net/client/rtmp/message/videoMessage.c",
        "ttLibC/ttLibC/net/client/rtmp/message/windowAcknowledgementSize.c",
        "ttLibC/ttLibC/net/client/rtmp/tetty/rtmpDecoder.c",
        "ttLibC/ttLibC/net/client/rtmp/tetty/rtmpEncoder.c",
        "ttLibC/ttLibC/net/client/rtmp/tetty/rtmpHandshake.c",
        "ttLibC/ttLibC/resampler/audioResampler.c",
        "ttLibC/ttLibC/resampler/imageResampler.c",
        "ttLibC/ttLibC/resampler/imageResizer.c",
        "ttLibC/ttLibC/resampler/soundtouchResampler.cpp",
        "ttLibC/ttLibC/resampler/speexdspResampler.c",
        "ttLibC/ttLibC/util/amfUtil.c",
        "ttLibC/ttLibC/util/beepUtil.c",
        "ttLibC/ttLibC/util/byteUtil.c",
        "ttLibC/ttLibC/util/crc32Util.c",
        "ttLibC/ttLibC/util/dynamicBufferUtil.c",
        "ttLibC/ttLibC/util/flvFrameUtil.c",
        "ttLibC/ttLibC/util/hexUtil.c",
        "ttLibC/ttLibC/util/ioUtil.c",
        "ttLibC/ttLibC/util/stlListUtil.cpp",
        "ttLibC/ttLibC/util/stlMapUtil.cpp",
        "csrc/ttLibC/net/tetty/bootstrap.cpp",
        "csrc/ttLibC/net/client/rtmp/rtmpBootstrap.cpp",
        "csrc/ttLibC/net/client/rtmp/tetty/rtmpClientHandler.cpp",
        "csrc/ttLibC/net/client/rtmp/tetty/rtmpCommandHandler.cpp"
      ],
      "libraries": [
        '<!@(node libcheck.js libs)'
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "ttLibC/",
        "./",
        '<!@(node libcheck.js includes)'
      ]
    },{
      "defines": [
        "<!@(node libcheck.js defs)",
        "__ENABLE_SOCKET__"
      ],
      "target_name": "ttLibJsGyp",
      "dependencies": ["ttLibC"], 
      "sources": [
        "csrc/ttLibJsGyp.cpp",
        "csrc/frame.cpp",
        "csrc/reader.cpp",
        "csrc/writer.cpp",
        "csrc/decoder.cpp",
        "csrc/decoder/avcodec.cpp",
        "csrc/encoder.cpp",
        "csrc/encoder/audioConverter.cpp",
        "csrc/encoder/faac.cpp",
        "csrc/encoder/jpeg.cpp",
        "csrc/encoder/mp3lame.cpp",
        "csrc/encoder/openh264.cpp",
        "csrc/encoder/opus.cpp",
        "csrc/encoder/theora.cpp",
        "csrc/encoder/vtCompressSession.cpp",
        "csrc/encoder/x264.cpp",
        "csrc/encoder/x265.cpp",
        "csrc/resampler.cpp",
        "csrc/resampler/audio.cpp",
        "csrc/resampler/image.cpp",
        "csrc/resampler/soundtouch.cpp",
        "csrc/resampler/speexdsp.cpp"
      ],
      "libraries": [
        '<!@(node libcheck.js libs)'
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "ttLibC/",
        "./",
        '<!@(node libcheck.js includes)'
      ]
    }
  ]
}
