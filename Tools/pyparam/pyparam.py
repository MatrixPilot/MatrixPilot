#!/usr/bin/env python

'''
Use pyparam to generate MatrixPilot mavlink parameter and nv memory tables from XML database description

Copyright Matthew Coleman 2012
Released under GNU GPL version 3 or later
'''

import os, sys, glob, re

import SubParameterDatabase as ParameterDB

# allow import from the MAVlink/pymavlink directory, where mavutil.py is
#sys.path.insert(0, os.path.join(os.path.dirname(os.path.realpath(__file__)), '../MAVLink/pymavlink'))


class ParameterTableGenerator():
    def __init__( self ):
        self.filePath = ""

    def openXMLDatabase( self ):
        self.ParamDBMain = ParameterDB.parse("ParameterDatabase.xml")

    def writeUDBTypesHeader( self ):
        headerFile = open(self.filePath + "../../MatrixPilot/parameter_datatypes.h", "w")
        headerFile.write("#ifndef PARAMETER_DATATYPES_H\r\n")
        headerFile.write("#define PARAMETER_DATATYPES_H\r\n")
        headerFile.write("// pyparam generated file - DO NOT EDIT\r\n\r\n")
#        headerFile.write('#include "parameter_table.h"\r\n')
        headerFile.write('#include "../MAVLink/include/mavlink_types.h"\r\n\r\n')
        storageFlags = self.ParamDBMain.get_serialisationFlags().get_serialisationFlag()
        headerFile.write("typedef enum\r\n\t{\r\n")
        index = 1 
        for storageFlag in storageFlags:
            headerFile.write("\tSTORAGE_FLAG_" + storageFlag + " = " + str(index) + ",\r\n" );
            index = index * 2
        headerFile.write("\t} storage_flags_e;\r\n\r\n")
        dataAreas = self.ParamDBMain.get_dataStorageAreas().get_dataStorageArea()
        headerFile.write("typedef enum\r\n\t{\r\n")
        index = 0      
        for dataArea in dataAreas:
            headerFile.write("\tSTORAGE_HANDLE_" + dataArea + " = " + str(index) + ",\r\n" );
            index = index + 1
        headerFile.write("\t} data_storage_handles_e;\r\n\r\n")
        dataTypes = self.ParamDBMain.get_udbTypes().get_udbType()
        headerFile.write("typedef enum\r\n\t{\r\n")
        for dataType in dataTypes:
            headerFile.write("\t" + dataType.get_typeName() + ",\r\n");
        headerFile.write("\t} udb_internal_type_e;\r\n\r\n")
        for dataType in dataTypes:
            headerFile.write("extern void " + dataType.get_sendFunction() + "(int16_t i);\r\n")
            headerFile.write("extern void " + dataType.get_setFunction() + "(mavlink_param_union_t setting, int16_t i);\r\n\r\n")
        headerFile.write("#endif // PARAMETER_DATATYPES_H\r\n")
        headerFile.close()

    def findMAVlinkParamType(self, udb_type):
        dataTypes = self.ParamDBMain.get_udbTypes().get_udbType()
        for dataType in dataTypes:
            if(dataType.get_typeName() == udb_type):
                return dataType.get_mavlinkType()
        return ""

    def writeParameterTable( self, which ):
        if which == 0:
            path = "../../MatrixPilot/parameter_table.c"
        else:
            path = "../../MatrixPilot/parameter_table2.c"
        tableFile = open(self.filePath + path, "w")
        tableFile.write("// pyparam generated file - DO NOT EDIT\r\n\r\n")
        tableFile.write('#include "defines.h"\r\n')
        tableFile.write('#include "mavlink_options.h"\r\n\r\n')
#        tableFile.write('#if(SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK) \r\n\r\n')
        tableFile.write('#if (SILSIM == ' + str(which) + ' && USE_MAVLINK == 1)\r\n\r\n')
        tableFile.write('#include "parameter_table.h"\r\n')
        tableFile.write('#include "data_storage.h"\r\n')
        dataTypes = self.ParamDBMain.get_udbTypes().get_udbType()
        paramBlocks = self.ParamDBMain.get_parameterBlocks().get_parameterBlock()
        for paramBlock in paramBlocks:
#            print(paramBlock.get_blockName());
            if(paramBlock.get_in_mavlink_parameters() == True):
                externs = paramBlock.get_externs()
                if(externs):
                    for extern in externs.get_externString():
                        tableFile.write('extern ' + extern + ';\r\n')
                includes = paramBlock.get_includes()
                if(includes):
                    for include in includes.get_includeString():
                        tableFile.write('#include "' + include + '"\r\n')
        tableFile.write('\r\n\r\n')
        tableFile.write('const mavlink_parameter_parser mavlink_parameter_parsers[] = {\r\n')
        for dataType in dataTypes:
            tableFile.write("\t{ &" + dataType.get_sendFunction() + ", &" + dataType.get_setFunction() + ", " + dataType.get_mavlinkType() + "},\r\n")
        tableFile.write("\t};\r\n\r\n")
        tableFile.write("#ifdef _MSC_VER\r\n")
        tableFile.write("mavlink_parameter mavlink_parameters_list[] = {\r\n")
        tableFile.write("#else\r\n")
        tableFile.write("const mavlink_parameter mavlink_parameters_list[] = {\r\n")
        tableFile.write("#endif // _MSC_VER\r\n")
        for paramBlock in paramBlocks:
#            print(paramBlock.get_blockName());
            if(paramBlock.get_in_mavlink_parameters() == True):
                for parameter in paramBlock.get_parameters().get_parameter():
                    tableFile.write('\t{"' + parameter.get_parameterName() + '", {')
                    if which == 0:
                        mavlinkType = self.findMAVlinkParamType(parameter.get_udb_param_type())
                        if(mavlinkType == "MAVLINK_TYPE_FLOAT"):
                            tableFile.write('.param_float=')
                        if(mavlinkType == "MAVLINK_TYPE_INT32_T"):
                            tableFile.write('.param_int32=')
                        if(mavlinkType == "MAVLINK_TYPE_UINT32_T"):
                            tableFile.write('.param_uint32=')
                    tableFile.write(parameter.get_min() + '}, {')
                    if which == 0:
                        if(mavlinkType == "MAVLINK_TYPE_FLOAT"):
                            tableFile.write('.param_float=')
                        if(mavlinkType == "MAVLINK_TYPE_INT32_T"):
                            tableFile.write('.param_int32=')
                        if(mavlinkType == "MAVLINK_TYPE_UINT32_T"):
                            tableFile.write('.param_uint32=')
                    tableFile.write(parameter.get_max() + '}, ' + parameter.get_udb_param_type() + ', ')
                    if(parameter.get_readonly() == "true"):
                        tableFile.write('PARAMETER_READONLY')
                    else:
                        tableFile.write('PARAMETER_READWRITE')
                    tableFile.write(', (void*)&' + parameter.get_variable_name() + ', sizeof(' + parameter.get_variable_name() + ') },\r\n')
            tableFile.write('\r\n')
        tableFile.write("};\r\n\r\n")
        tableFile.write("const uint16_t count_of_parameters_list = sizeof(mavlink_parameters_list) / sizeof(mavlink_parameter);\r\n\r\n\r\n")
        tableFile.write('#endif  // (SILSIM == ' + str(which) + ' && USE_MAVLINK == 1)\r\n')
        tableFile.close()

    def writeParameterTableInit( self ):
        count = 0;
        tableFile = open(self.filePath + "../../MatrixPilot/parameter_table_init.c", "w")
        tableFile.write("// pyparam generated file - DO NOT EDIT\r\n\r\n")
        tableFile.write("// this module is a horrible hack to work around VC++ not allowing\r\n") 
        tableFile.write("// static initialisation of named union member variables\r\n")
        tableFile.write('#ifdef _MSC_VER\r\n\r\n')
        tableFile.write('#include "defines.h" \r\n')
        tableFile.write('#include "mavlink_options.h"\r\n\r\n')
        tableFile.write('#if (USE_MAVLINK == 1)\r\n\r\n')
        tableFile.write('#include "parameter_table.h"\r\n')
        tableFile.write('#include "data_storage.h"\r\n')
        tableFile.write('\r\n')
        paramBlocks = self.ParamDBMain.get_parameterBlocks().get_parameterBlock()
        tableFile.write('void parameter_table_init(void)\r\n{\r\n')
        for paramBlock in paramBlocks:
            if(paramBlock.get_in_mavlink_parameters() == True):
                for parameter in paramBlock.get_parameters().get_parameter():
                    strType = self.findMAVlinkParamType(parameter.get_udb_param_type())
                    if strType == 'MAVLINK_TYPE_INT32_T':
                        strType = 'param_int32'
                    else:
                        strType = 'param_float'
                    tableFile.write('\tmavlink_parameters_list[' + str(count) + '].min.' + strType +'=' + parameter.get_min())
                    tableFile.write('; mavlink_parameters_list[' + str(count) + '].max.' + strType +'=' + parameter.get_max())
                    tableFile.write('; // ' + parameter.get_variable_name() + ' - ' + parameter.get_parameterName() + '\r\n')
                    count = count + 1
            tableFile.write('\r\n')
        tableFile.write("};\r\n\r\n")
        tableFile.write('#endif // (USE_MAVLINK == 1)\r\n')
        tableFile.write('#endif // _MSC_VER\r\n')
        tableFile.close()


    def writeStorageTable( self ):
        tableFile = open(self.filePath + "../../MatrixPilot/nv_memory_table.c", "w")
        tableFile.write("// pyparam generated file - DO NOT EDIT\r\n\r\n\r\n")
        tableFile.write('#include "parameter_table.h"\r\n')
        tableFile.write('#include "data_services.h"\r\n\r\n')
        tableFile.write('#if(USE_NV_MEMORY == 1)\r\n\r\n')
        dataTypes = self.ParamDBMain.get_udbTypes().get_udbType()
        paramBlocks = self.ParamDBMain.get_parameterBlocks().get_parameterBlock()
        param_index = 0;
        # Sizes of parameter blocks  
        paramBlockSizes = []
        tableFile.write('const mavlink_parameter_block mavlink_parameter_blocks[] = {\r\n')
        for paramBlock in paramBlocks:
#            print(paramBlock.get_blockName());
            if(paramBlock.get_in_mavlink_parameters() == True):
                end_index = param_index + len(paramBlock.get_parameters().get_parameter())
                tableFile.write("\t{ STORAGE_HANDLE_" + paramBlock.get_storage_area() + ", " + str(param_index)  + ", " + str(end_index - param_index) + ", ")
                first = True
                for serialisationFlag in paramBlock.get_serialisationFlags().get_serialisationFlag():
                    if(first == False):
                        tableFile.write(" | ")
                    tableFile.write("STORAGE_FLAG_" + serialisationFlag)
                    first = False
                if(paramBlock.get_load_callback() != "NULL"):
                    tableFile.write( ", &" + paramBlock.get_load_callback() + ' },\r\n')
                else:
                    tableFile.write( ", NULL },\r\n")
                param_index = end_index
        tableFile.write("};\r\n\r\n\r\n")    
        tableFile.write("const uint16_t mavlink_parameter_block_count = sizeof(mavlink_parameter_blocks) / sizeof(mavlink_parameter_block);\r\n\r\n")
        tableFile.write('#endif // USE_NV_MEMORY\r\n\r\n')
        tableFile.close()


paramGen = ParameterTableGenerator()
paramGen.openXMLDatabase()
paramGen.writeUDBTypesHeader()
paramGen.writeParameterTable(0)
paramGen.writeParameterTable(1)
paramGen.writeParameterTableInit()
paramGen.writeStorageTable()
