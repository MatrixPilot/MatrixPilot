// This file is released under the 3-clause BSD license. See COPYING-BSD.

if getos() == "Windows" then
    // to manage long pathname
    includes_src_c = "-I""" + get_absolute_file_path("builder_gateway_c.sce") + "../../src/c""";
else
    includes_src_c = "-I" + get_absolute_file_path("builder_gateway_c.sce") + "../../src/c";
end

// PutLhsVar managed by user in sci_sum and in sci_sub
// if you do not this variable, PutLhsVar is added
// in gateway generated (default mode in scilab 4.x and 5.x)
WITHOUT_AUTO_PUTLHSVAR = %t;

tbx_build_gateway("mavlink_toolbox_c", ..
                  [], ..
                  [], ..
                  get_absolute_file_path("builder_gateway_c.sce"), ..
                  [], ..
                  "", ..
                  includes_src_c);

clear WITHOUT_AUTO_PUTLHSVAR;

clear tbx_build_gateway;
