// This file is released under the 3-clause BSD license. See COPYING-BSD.

// This macro compiles the files

src_c_path = get_absolute_file_path("builder_c.sce");

CFLAGS = "-I" + src_c_path;
LDFLAGS = "";
if (getos()<>"Windows") then
    if ~isdir(SCI+"/../../share") then
        // Source version
        CFLAGS = CFLAGS + " -I" + SCI + "/modules/scicos_blocks/includes" ;
        CFLAGS = CFLAGS + " -I" + SCI + "/modules/scicos/includes" ;
        CFLAGS = CFLAGS + " -I" + src_c_path + "/MAVLink/include" ;
//        CFLAGS = CFLAGS + " -I" + src_c_path + "../../../../../MAVLink/include/matrixpilot" ;
//        CFLAGS = CFLAGS + " -I" + src_c_path + "../../../../../MAVLink/include/common" ;
    else
        // Release version
        CFLAGS = CFLAGS + " -I" + SCI + "/../../include/scilab/scicos_blocks";
        CFLAGS = CFLAGS + " -I" + SCI + "/../../include/scilab/scicos";
        CFLAGS = CFLAGS + " -I" + src_c_path + "/MAVLink/include" ;
//        CFLAGS = CFLAGS + " -I" + src_c_path + "../../../../../MAVLink/include/matrixpilot" ;
//        CFLAGS = CFLAGS + " -I" + src_c_path + "../../../../../MAVLink/include/common" ;
    end
else
    CFLAGS = CFLAGS + " -I" + SCI + "/modules/scicos_blocks/includes";
    CFLAGS = CFLAGS + " -I" + SCI + "/modules/scicos/includes";
    CFLAGS = CFLAGS + " -I" + src_c_path + "/msvc" ;
    CFLAGS = CFLAGS + " -I" + src_c_path + "/MAVLink/include" ;
//    CFLAGS = CFLAGS + " -I" + src_c_path + "../../../../../MAVLink/include/matrixpilot" ;
//    CFLAGS = CFLAGS + " -I" + src_c_path + "../../../../../MAVLink/include/common" ;

    // Getting symbols
    if findmsvccompiler() <> "unknown" & haveacompiler() then
        LDFLAGS = LDFLAGS + " """ + SCI + "/bin/scicos.lib""";
        LDFLAGS = LDFLAGS + " """ + SCI + "/bin/scicos_f.lib""";
        LDFLAGS = LDFLAGS + " Ws2_32.lib";
    end
end

tbx_build_src(["mavlink_receive","startServer","imu_basics"],       ..
              ["mavlink_slb.c","udp_server.c","imu_basics.c"],   ..
              "c",                              ..
              src_c_path,                       ..
              "",                               ..
              LDFLAGS,                          ..
              CFLAGS);

clear tbx_build_src;
clear src_c_path;
clear CFLAGS LDFLAGS;

