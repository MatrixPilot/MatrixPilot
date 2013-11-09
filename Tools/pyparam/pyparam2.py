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
        self.ParamDBMain = ParameterDB.parse("ParameterDatabase2.xml")


    def writeUDBTypesHeader( self ):
        headerFile = open(self.filePath + "../../MatrixPilot/parameter_datatypes.h", "w")
        
        headerFile.write("#ifndef PARAMETER_DATATYPES_H\n")
        headerFile.write("#define PARAMETER_DATATYPES_H\n")

        headerFile.write("// pyparam generated file - DO NOT EDIT\n\n")
        
#        headerFile.write('#include "parameter_table.h"\n')
        headerFile.write('#include "../MAVLink/include/mavlink_types.h"\n\n')

        storageFlags = self.ParamDBMain.get_serialisationFlags().get_serialisationFlag()
        headerFile.write("typedef enum\n    {\n")
        index = 1 
        for storageFlag in storageFlags:
            headerFile.write("    STORAGE_FLAG_" + storageFlag + " = " + str(index) + ",\n" );
            index = index * 2
        headerFile.write("    } storage_flags_e;\n\n")
        
 
        dataAreas = self.ParamDBMain.get_dataStorageAreas().get_dataStorageArea()
        headerFile.write("typedef enum\n    {\n")
        index = 0      
        for dataArea in dataAreas:
            headerFile.write("    STORAGE_HANDLE_" + dataArea + " = " + str(index) + ",\n" );
            index = index + 1
        headerFile.write("    } data_storage_handles_e;\n\n")
        
        
        dataTypes = self.ParamDBMain.get_udbTypes().get_udbType()
        headerFile.write("typedef enum\n    {\n")
        for dataType in dataTypes:
            headerFile.write("    " + dataType.get_typeName() + ",\n");
        headerFile.write("    } udb_internal_type_e;\n\n")
            
        for dataType in dataTypes:
            headerFile.write("extern void " + dataType.get_sendFunction() + "( int16_t i ) ;\n")
            headerFile.write("extern void " + dataType.get_setFunction() + "(mavlink_param_union_t setting, int16_t i ) ;\n\n")
            
    
        headerFile.write("#endif    // PARAMETER_DATATYPES_H\n")
        
        headerFile.close()

        
    def findMAVlinkParamType(self, udb_type):
        dataTypes = self.ParamDBMain.get_udbTypes().get_udbType()
        for dataType in dataTypes:
            if(dataType.get_typeName() == udb_type):
                return dataType.get_mavlinkType()
        return ""
        


    def writeParameterTable( self ):
        tableFile = open(self.filePath + "../../MatrixPilot/parameter_table.c", "w")
        
        tableFile.write("// pyparam generated file - DO NOT EDIT\n\n")
        
        tableFile.write('#include "defines.h" \n\n')        
        tableFile.write('#if(SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK) \n\n')
        
        tableFile.write('#include "parameter_table.h"\n')
        tableFile.write('#include "data_storage.h"\n\n\n')
        
        dataTypes = self.ParamDBMain.get_udbTypes().get_udbType()
        paramBlocks = self.ParamDBMain.get_parameterBlocks().get_parameterBlock()
        
        for paramBlock in paramBlocks:
#            print(paramBlock.get_blockName());
            if(paramBlock.get_in_mavlink_parameters() == True):
                externs = paramBlock.get_externs()
            
                if(externs):
                    for extern in externs.get_externString():
                        tableFile.write('extern ' + extern + ' ;\n')        

        tableFile.write('\n\n')
   
       
        tableFile.write('const mavlink_parameter_parser    mavlink_parameter_parsers[] = {\n')
        
        for dataType in dataTypes:
            tableFile.write("    { &" + dataType.get_sendFunction() + ", &" + dataType.get_setFunction() + ", " + dataType.get_mavlinkType() + "},\n")
            
        tableFile.write("    };\n\n")


        tableFile.write("const mavlink_parameter mavlink_parameters_list[] = {\n ")

        for paramBlock in paramBlocks:
#            print(paramBlock.get_blockName());
            if(paramBlock.get_in_mavlink_parameters() == True):
                for parameter in paramBlock.get_parameters().get_parameter():
                    tableFile.write('    {"' + parameter.get_parameterName() + '" , {.')
                    mavlinkType = self.findMAVlinkParamType(parameter.get_udb_param_type())
                    
                    if(mavlinkType == "MAVLINK_TYPE_FLOAT"):
                        tableFile.write('param_float=')
                    if(mavlinkType == "MAVLINK_TYPE_INT32_T"):
                        tableFile.write('param_int32=')
                    if(mavlinkType == "MAVLINK_TYPE_UINT32_T"):
                        tableFile.write('param_uint32=')
    
                    tableFile.write( parameter.get_min() + '} , {.')
    
                    if(mavlinkType == "MAVLINK_TYPE_FLOAT"):
                        tableFile.write('param_float=')
                    if(mavlinkType == "MAVLINK_TYPE_INT32_T"):
                        tableFile.write('param_int32=')
                    if(mavlinkType == "MAVLINK_TYPE_UINT32_T"):
                        tableFile.write('param_uint32=')
                        
                    tableFile.write( parameter.get_max() + '} , ' + parameter.get_udb_param_type() + ', ')
                    
                    if(parameter.get_readonly() == "true"):
                        tableFile.write('PARAMETER_READONLY')
                    else:
                        tableFile.write('PARAMETER_READWRITE')
                    
                    tableFile.write(', (void*) &' + parameter.get_variable_name() + ', sizeof(' + parameter.get_variable_name() + ') },\n')
            tableFile.write('\n')
        tableFile.write("    };\n\n")

        tableFile.write("const uint16_t count_of_parameters_list = sizeof(mavlink_parameters_list) / sizeof(mavlink_parameter);\n\n\n")
        
        tableFile.write('#endif \n\n')        

        tableFile.close()


    def writeStorageTable( self ):
        tableFile = open(self.filePath + "../../MatrixPilot/nv_memory_table.c", "w")
        
        tableFile.write("// pyparam generated file - DO NOT EDIT\n\n\n")
        
        tableFile.write('#include "parameter_table.h"\n')
        tableFile.write('#include "data_services.h"\n\n')

        tableFile.write('#if(USE_NV_MEMORY == 1)\n\n')
        
        dataTypes = self.ParamDBMain.get_udbTypes().get_udbType()
        paramBlocks = self.ParamDBMain.get_parameterBlocks().get_parameterBlock()
        
        param_index = 0;
        
        # Sizes of parameter blocks  
        paramBlockSizes = []

        tableFile.write('const mavlink_parameter_block    mavlink_parameter_blocks[] = {\n')

        for paramBlock in paramBlocks:
#            print(paramBlock.get_blockName());
            if(paramBlock.get_in_mavlink_parameters() == True):
                
                end_index = param_index + len(paramBlock.get_parameters().get_parameter())
                
                tableFile.write("    { STORAGE_HANDLE_" + paramBlock.get_storage_area() + " , " + str(param_index)  + " , " + str(end_index - param_index) + " , ")
                
                first = True
                
                for serialisationFlag in paramBlock.get_serialisationFlags().get_serialisationFlag():
                    if(first == False):
                        tableFile.write(" | ")
                            
                    tableFile.write("STORAGE_FLAG_" + serialisationFlag)
                    first = False

                if(paramBlock.get_load_callback() != "NULL"):
                    tableFile.write( " , &" + paramBlock.get_load_callback() + ' , ')
                else:
                    tableFile.write( " , NULL , ")
                    
                tableFile.write( '"' + paramBlock.get_storage_area() + '" ' )
                tableFile.write(" },\n ")
                
                param_index = end_index
            
        tableFile.write("    };\n\n\n")    

        tableFile.write("const uint16_t mavlink_parameter_block_count = sizeof(mavlink_parameter_blocks) / sizeof(mavlink_parameter_block);\n\n")

        tableFile.write('#endif    //USE_NV_MEMORY\n\n')


        tableFile.close()


paramGen = ParameterTableGenerator()
paramGen.openXMLDatabase()
paramGen.writeUDBTypesHeader()
paramGen.writeParameterTable()
paramGen.writeStorageTable()


        
        
