var tt = require("..");
tt.MsSetup.CoInitialize("multiThreaded");
tt.MsSetup.MFStartup();
var encoder = new tt.encoder.MSH264Encoder("H264 Encoder MF", 320, 240, 320000);
// うーん、丸Rですか・・・でもこれじゃないと、文字コードあわないんだよねぇ・・・
// 文字化けの温床にしかなさそうな気がする・・・
//var encoder = new tt.encoder.MSH264Encoder("Intel® Quick Sync Video H.264 Encoder MF", 320, 240, 320000);

setInterval(() => {
  encoder.encode(null, (err, frame) => {
    console.log("encodeされました。");
  });
}, 100);
// 終了時には、ちゃんとshutdownとuninitializeすべきではある。
/*
tt.MsSetup.MFShutdown();
tt.MsSetup.CoUninitialize();
*/