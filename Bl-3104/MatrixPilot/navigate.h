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

#ifndef NAVIGATE_H
#define NAVIGATE_H


struct waypointparameters {
	int16_t x;
	int16_t y;
	int16_t cosphi;
	int16_t sinphi;
	int8_t  phi;
	int16_t height;
	int16_t fromHeight;
	int16_t legDist;
};

extern struct waypointparameters goal;
extern struct relative2D togoal;
extern int16_t tofinish_line;
extern int16_t progress_to_goal; // Fraction of the way to the goal in the range 0-4096 (2^12)
extern int8_t desired_dir;

void init_navigation(void);
#ifdef USE_EXTENDED_NAV
void set_goal(struct relative3D_32 fromPoint, struct relative3D_32 toPoint);
#else
void set_goal(struct relative3D fromPoint , struct relative3D toPoint);
#endif // USE_EXTENDED_NAV
void update_goal_alt(int16_t z);
void compute_bearing_to_goal (void);
void process_flightplan(void);
int16_t determine_navigation_deflection(char navType);


#endif // NAVIGATE_H
