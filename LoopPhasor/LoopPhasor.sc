// LoopPhasor is a variant of Phasor with the following changes:
// 1. It has an embedded loop with start and end position (for playing samples with loop points).
//    This allows the LoopPhasor to be used for playing a Buffer normally, and then you only loop within a subset of the Buffer. 
// 2. There are two triggers. One is a trigger for returning to the start position. The other triggers an end to the looping behavior.
LoopPhasor : UGen {
    *ar { arg trigStart = 0.0, trigEnd = 0.0, rate = 1.0, start = 0.0, end = 1.0, loopStart = 0.0, loopEnd = 1.0;
        ^this.multiNew('audio', trigStart, trigEnd, rate, start, end, loopStart, loopEnd);
    }
    *kr { arg trigStart = 0.0, trigEnd = 0.0, rate = 1.0, start = 0.0, end = 1.0, loopStart = 0.0, loopEnd = 1.0;
        ^this.multiNew('control', trigStart, trigEnd, rate, start, end, loopStart, loopEnd);
    }
}
