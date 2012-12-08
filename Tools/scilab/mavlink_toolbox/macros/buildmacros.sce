// This file is released under the 3-clause BSD license. See COPYING-BSD.

function buildmacros()
    tbx_build_macros(TOOLBOX_NAME,get_absolute_file_path("buildmacros.sce"));
    tbx_build_blocks(toolbox_dir, ["MAVLINK_RECEIVE" "IMU_BASICS"]);
endfunction

buildmacros();
clear buildmacros; // remove buildmacros on stack
