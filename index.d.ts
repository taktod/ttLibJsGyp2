/// <reference types="node"/>
declare module 'ttlibjsgyp' {
  export class Frame {
    public type:string;
    public pts:number;
    public timebase:number;
    public id:number;
    public width?:number;
    public height?:number;
    public videoType?:string;
    public sampleRate?:number;
    public sampleNum?:number;
    getBinaryBuffer():Buffer;
  }
  export namespace reader {
    export class FlvReader {
      constructor();
      readFrame(data:Buffer, func:{(err:string, frame:Frame):boolean}):boolean;
    }
    export class MkvReader {
      constructor();
      readFrame(data:Buffer, func:{(err:string, frame:Frame):boolean}):boolean;
    }
    export class Mp4Reader {
      constructor();
      readFrame(data:Buffer, func:{(err:string, frame:Frame):boolean}):boolean;
    }
    export class MpegtsReader {
      constructor();
      readFrame(data:Buffer, func:{(err:string, frame:Frame):boolean}):boolean;
    }
    export class WebmReader {
      constructor();
      readFrame(data:Buffer, func:{(err:string, frame:Frame):boolean}):boolean;
    }
  }
  export namespace writer {
    export class FlvWriter {
      constructor(videoCodec:string, audioCodec:string);
      writeFrame(frame:Frame, func:{(err:string, data:Buffer):boolean}):boolean;
      setMode(mode:number):boolean;
    }
    export class MkvWriter {
      constructor(unitDuration:number, ...codecs:string);
      writeFrame(frame:Frame, func:{(err:string, data:Buffer):boolean}):boolean;
      setMode(mode:number):boolean;
    }
    export class Mp4Writer {
      constructor(unitDuration:number, ...codecs:string);
      writeFrame(frame:Frame, func:{(err:string, data:Buffer):boolean}):boolean;
      setMode(mode:number):boolean;
    }
    export class MpegtsWriter {
      constructor(unitDuration:number, ...codecs:string);
      writeFrame(frame:Frame, func:{(err:string, data:Buffer):boolean}):boolean;
      writeInfo(func:{(err:string, data:Buffer):boolean}):boolean;
      setMode(mode:number):boolean;
    }
    export class WebmWriter {
      constructor(unitDuration:number, ...codecs:string);
      writeFrame(frame:Frame, func:{(err:string, data:Buffer):boolean}):boolean;
      setMode(mode:number):boolean;
    }
  }
}
