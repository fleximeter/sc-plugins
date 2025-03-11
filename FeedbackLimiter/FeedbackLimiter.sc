// File: FeedbackLimiter.sc
// Author: Jeff Martin
//
// Description:
// This is a SuperColllider UGen for stabilizing audio feedback based on current amplitude
//
// Copyright © 2025 by Jeffrey Martin. All rights reserved.
// Website: https://www.jeffreymartincomposer.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

FeedbackLimiter : UGen {
    *ar { arg trigStart = 0.0, trigEnd = 0.0, rate = 1.0, start = 0.0, end = 1.0, loopStart = 0.0, loopEnd = 1.0;
        ^this.multiNew('audio', trigStart, trigEnd, rate, start, end, loopStart, loopEnd);
    }
    *kr { arg trigStart = 0.0, trigEnd = 0.0, rate = 1.0, start = 0.0, end = 1.0, loopStart = 0.0, loopEnd = 1.0;
        ^this.multiNew('control', trigStart, trigEnd, rate, start, end, loopStart, loopEnd);
    }
}

Amplitude : UGen {
	*ar { arg in = 0.0, attackTime = 0.01, releaseTime = 0.01, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, attackTime, releaseTime).madd(mul, add)
	}
	*kr { arg in = 0.0, attackTime = 0.01, releaseTime = 0.01, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, attackTime, releaseTime).madd(mul, add)
	}
}