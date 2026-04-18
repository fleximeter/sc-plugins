// File: PV_CFreeze.sc
// Author: Jeff Martin
//
// Description:
// This is a SuperColllider UGen based on the PV_MagFreeze and PV_Freeze classes.
// It produces a more flexible phase vocoder spectral freeze patterned after
// Jean-François Charles's Max implementation.
// 
// Copyright © 2026 by Jeffrey Martin. All rights reserved.
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

// RubberBandPS is a phase vocoder based pitch shifter using the Rubber Band library.
RubberBandPS : UGen {
    *ar {
        arg in, pitchRatio=1.0, formantRatio=1.0, mul=1.0, add=0.0;
        ^this.multiNew('audio', in, pitchRatio, formantRatio).madd(mul, add);
    }
}
