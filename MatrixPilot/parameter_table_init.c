// this module is a horrible hack to work around VC++ not allowing 
// static initialisation of named union member variables
#ifdef _MSC_VER

#include "defines.h" 

#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK) 

#include "parameter_table.h"
#include "data_storage.h"

void parameter_table_init(void)
{
mavlink_parameters_list[0].min.param_float=0.0;	mavlink_parameters_list[0].max.param_float=0.5;
mavlink_parameters_list[1].min.param_float=0.0;	mavlink_parameters_list[1].max.param_float=0.5;
mavlink_parameters_list[2].min.param_float=0.0;	mavlink_parameters_list[2].max.param_float=0.5;
mavlink_parameters_list[3].min.param_float=0.0;	mavlink_parameters_list[3].max.param_float=0.5;
mavlink_parameters_list[4].min.param_float=0.0;	mavlink_parameters_list[4].max.param_float=0.5;
mavlink_parameters_list[5].min.param_float=0.0;	mavlink_parameters_list[5].max.param_float=0.5;
mavlink_parameters_list[6].min.param_float=0.0;	mavlink_parameters_list[6].max.param_float=0.5;
mavlink_parameters_list[7].min.param_float=0.0;	mavlink_parameters_list[7].max.param_float=0.5;
mavlink_parameters_list[8].min.param_float=0.0;	mavlink_parameters_list[8].max.param_float=0.5;
mavlink_parameters_list[9].min.param_float=0.0;	mavlink_parameters_list[9].max.param_float=0.5;
mavlink_parameters_list[10].min.param_float=0.0;	mavlink_parameters_list[10].max.param_float=0.5;
mavlink_parameters_list[11].min.param_float=0.0;	mavlink_parameters_list[11].max.param_float=0.5;
mavlink_parameters_list[12].min.param_int32=-32767;	mavlink_parameters_list[12].max.param_int32=32767;
mavlink_parameters_list[13].min.param_int32=-32767;	mavlink_parameters_list[13].max.param_int32=32767;
mavlink_parameters_list[14].min.param_int32=-32767;	mavlink_parameters_list[14].max.param_int32=32767;
mavlink_parameters_list[15].min.param_int32=-32767;	mavlink_parameters_list[15].max.param_int32=32767;
mavlink_parameters_list[16].min.param_int32=-32767;	mavlink_parameters_list[16].max.param_int32=32767;
mavlink_parameters_list[17].min.param_int32=-32767;	mavlink_parameters_list[17].max.param_int32=32767;
mavlink_parameters_list[18].min.param_int32=-32767;	mavlink_parameters_list[18].max.param_int32=32767;
mavlink_parameters_list[19].min.param_int32=-32767;	mavlink_parameters_list[19].max.param_int32=32767;
mavlink_parameters_list[20].min.param_int32=-32767;	mavlink_parameters_list[20].max.param_int32=32767;
mavlink_parameters_list[21].min.param_int32=-180;	mavlink_parameters_list[21].max.param_int32=180;
mavlink_parameters_list[22].min.param_float=800.0;	mavlink_parameters_list[22].max.param_float=2200.0;
mavlink_parameters_list[23].min.param_float=800.0;	mavlink_parameters_list[23].max.param_float=2200.0;
mavlink_parameters_list[24].min.param_float=800.0;	mavlink_parameters_list[24].max.param_float=2200.0;
mavlink_parameters_list[25].min.param_float=800.0;	mavlink_parameters_list[25].max.param_float=2200.0;
mavlink_parameters_list[26].min.param_float=800.0;	mavlink_parameters_list[26].max.param_float=2200.0;
mavlink_parameters_list[27].min.param_float=800.0;	mavlink_parameters_list[27].max.param_float=2200.0;
mavlink_parameters_list[28].min.param_float=800.0;	mavlink_parameters_list[28].max.param_float=2200.0;
mavlink_parameters_list[29].min.param_float=800.0;	mavlink_parameters_list[29].max.param_float=2200.0;
mavlink_parameters_list[30].min.param_float=800.0;	mavlink_parameters_list[30].max.param_float=2200.0;
mavlink_parameters_list[31].min.param_float=800.0;	mavlink_parameters_list[31].max.param_float=2200.0;
mavlink_parameters_list[32].min.param_float=800.0;	mavlink_parameters_list[32].max.param_float=2200.0;
mavlink_parameters_list[33].min.param_float=800.0;	mavlink_parameters_list[33].max.param_float=2200.0;
mavlink_parameters_list[34].min.param_float=800.0;	mavlink_parameters_list[34].max.param_float=2200.0;
mavlink_parameters_list[35].min.param_float=800.0;	mavlink_parameters_list[35].max.param_float=2200.0;
mavlink_parameters_list[36].min.param_float=800.0;	mavlink_parameters_list[36].max.param_float=2200.0;
mavlink_parameters_list[37].min.param_int32=-32767;	mavlink_parameters_list[37].max.param_int32=32767;
mavlink_parameters_list[38].min.param_int32=-32767;	mavlink_parameters_list[38].max.param_int32=32767;
mavlink_parameters_list[39].min.param_int32=-32767;	mavlink_parameters_list[39].max.param_int32=32767;
mavlink_parameters_list[40].min.param_int32=-32767;	mavlink_parameters_list[40].max.param_int32=32767;
mavlink_parameters_list[41].min.param_int32=-32767;	mavlink_parameters_list[41].max.param_int32=32767;
mavlink_parameters_list[42].min.param_int32=-32767;	mavlink_parameters_list[42].max.param_int32=32767;
mavlink_parameters_list[43].min.param_int32=-32767;	mavlink_parameters_list[43].max.param_int32=32767;
mavlink_parameters_list[44].min.param_int32=0;	mavlink_parameters_list[44].max.param_int32=1;
mavlink_parameters_list[45].min.param_int32=0;	mavlink_parameters_list[45].max.param_int32=1;
mavlink_parameters_list[46].min.param_int32=1;	mavlink_parameters_list[46].max.param_int32=500;
mavlink_parameters_list[47].min.param_float=0;	mavlink_parameters_list[47].max.param_float=1;
mavlink_parameters_list[48].min.param_float=0;	mavlink_parameters_list[48].max.param_float=1;
mavlink_parameters_list[49].min.param_int32=-89;	mavlink_parameters_list[49].max.param_int32=0;
mavlink_parameters_list[50].min.param_int32=0;	mavlink_parameters_list[50].max.param_int32=89;
mavlink_parameters_list[51].min.param_int32=0;	mavlink_parameters_list[51].max.param_int32=89;
mavlink_parameters_list[52].min.param_int32=0;	mavlink_parameters_list[52].max.param_int32=89;
mavlink_parameters_list[53].min.param_float=0;	mavlink_parameters_list[53].max.param_float=300.0;
mavlink_parameters_list[54].min.param_float=0;	mavlink_parameters_list[54].max.param_float=20000;
mavlink_parameters_list[55].min.param_float=0;	mavlink_parameters_list[55].max.param_float=300.0;
mavlink_parameters_list[56].min.param_float=0;	mavlink_parameters_list[56].max.param_float=300.0;
mavlink_parameters_list[57].min.param_float=0;	mavlink_parameters_list[57].max.param_float=300.0;
mavlink_parameters_list[58].min.param_int32=-90;	mavlink_parameters_list[58].max.param_int32=90.0;
mavlink_parameters_list[59].min.param_int32=-90;	mavlink_parameters_list[59].max.param_int32=90.0;
mavlink_parameters_list[60].min.param_int32=1.0;	mavlink_parameters_list[60].max.param_int32=720.0;
mavlink_parameters_list[61].min.param_float=0.0;	mavlink_parameters_list[61].max.param_float=1.0;
mavlink_parameters_list[62].min.param_int32=0.0;	mavlink_parameters_list[62].max.param_int32=45.0;
}

#endif // SERIAL_OUTPUT_FORMAT
#endif // _MSC_VER
