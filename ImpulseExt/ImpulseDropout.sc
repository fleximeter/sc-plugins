// File: ImpulseDropout.sc
// Author: Jeff Martin
//
// Description:
// This is a SuperColllider UGen based on the Impulse class.
// It allows random dropout of a percentage of impulses.
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

// ImpulseDropout is a version of Impulse that randomly drops a percentage of the impulses.
ImpulseDropout : UGen {
    *ar {
        arg freq = 440.0, phase = 0.0, dropFrac = 0.0, mul = 1.0, add = 0.0;
        ^this.multiNew('audio', freq, phase, dropFrac).madd(mul, add);
    }
    *kr {
        arg freq = 440.0, phase = 0.0, dropFrac = 0.0, mul = 1.0, add = 0.0;
        ^this.multiNew('control', freq, phase, dropFrac).madd(mul, add);
    }
    signalRange { ^\unipolar }
}
