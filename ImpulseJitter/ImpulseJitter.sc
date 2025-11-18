// File: ImpulseJitter.sc
// Author: Jeff Martin
//
// Description:
// This is a SuperColllider UGen based on the Impulse class.
// It allows adding a stochastic element to the impulse position.
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

// ImpulseJitter is a version of Impulse that allows the addition of jitter to each impulse.
ImpulseJitter : UGen {
    *ar {
        arg freq = 440.0, phase = 0.0, jitterFrac = 0.0, mul = 1.0, add = 0.0;
        ^this.multiNew('audio', freq, phase, jitterFrac).madd(mul, add);
    }
    *kr {
        arg freq = 440.0, phase = 0.0, jitterFrac = 0.0, mul = 1.0, add = 0.0;
        ^this.multiNew('control', freq, phase, jitterFrac).madd(mul, add);
    }
    signalRange { ^\unipolar }
}
