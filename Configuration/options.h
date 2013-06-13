// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2012 MatrixPilot Team
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


// perhaps all these files should be prefixed/suffixed with _options_/_config_ ??
//  as i don't want these names to ever conflict with a source code module
//  (or potential module) of the same name


// describes the type and setup of the autopilot board (hardware configuration)
#include "hardware_config.h"   // maybe this would be better called setup.h ?

// defines the remote control I/O and analog input channel definitions
#include "channels_config.h" // i also considered rcio_config.h (remote control input output)

// enables/disables features of MatrixPilot
#include "features_config.h" // aiming for this to have all the USE_XXX defines, otherwise not have it at all and these enabling defines go in each features own xxx_config.h

// configure altitude hold options - combine with airspeed_options.h?
#include "altitude_config.h"

// parameters for HILSIM and SILSIM build methods
//#include "simulate_config.h"

// all the control loop gains (PID loop tuning)
#include "gains_config.h" // or tuning.h

// optional camera setup
#include "camera_config.h"

// TCP/IP network configuration and options
#include "network_config.h"

// Want to integrate this with existing mavlink_options.h
#include "mavlink_config.h" // and the other consideration is telemetry.h

// combine with osd_layout.h?
#include "osd_config.h"


////////////////////////////////////////////////////////////////////////////////
// Developers may wish to override options above in a local working copy
// Useful so as to aid easily keeping this file in sync on SVN
// Add options_local.h to .svnignore to avoid conflicts
//
// options_local.h in the SVN repository should always remain empty
//
//#include "options_local.h"

