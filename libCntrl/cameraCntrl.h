// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _CAMERA_CNTRL_H_
#define _CAMERA_CNTRL_H_

void set_camera_view(struct relative3D current_view);
void compute_camera_view(void);
void cameraCntrl(void);

void camera_live_begin(void);
void camera_live_received_byte(uint8_t inbyte);
void camera_live_commit(void);
void camera_live_commit_values(const struct relative3D target);

//#define CAM_VIEW_LAUNCH     { 0, 0, 0 }

#endif // _CAMERA_CNTRL_H_
