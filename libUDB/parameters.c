#include "../libUDB/libUDB.h"
#include "parameters.h"
#include "../MatrixPilot/parameter_table.h"
#include <string.h>

/**
 * Array of static parameter info.
 */

int16_t zip_bag = 999;
fractional bluff = 12345;
//int16_t misc_storage_flags = 665;

PARAM_DEFINE_START();
PARAM_DEFINE_END();

PARAM_SECTION(MISC, 0, NULL);

//PARAM_DEFINE_INT(PARAM_SF_NAME, &misc_storage_flags, 0, 32767, false);

PARAM_DEFINE_INT(MISC_ZIP_BAG, &zip_bag, 0, RMAX, false);
PARAM_DEFINE_Q14(MISC_BLUFF, &bluff, -1.0, 1.0, false);

PARAM_SECTION_START();
PARAM_SECTION_END();

extern char __param_start, __param_end;
static const struct param_info_s	*param_info_base = &(((struct param_info_s *) &__param__the_start)[1]);
static const struct param_info_s	*param_info_limit = (struct param_info_s *) &__param__the_end;
#define	param_info_count		((unsigned)(&param_info_limit - &param_info_base))


static const struct param_section_s	*param_section_base = &(((struct param_section_s *) &__section__the_start)[1]);
static const struct param_section_s	*param_section_limit = (struct param_section_s *) &__section__the_end;
#define	param_section_count		((unsigned)(&param_section_limit - &param_section_base))


uint16_t get_param_count(void)
{
    return (&__param__the_end - &__param__the_start) - 1;
};

uint16_t get_active_param_count(boolean telemetry)
{
    return (&__param__the_end - &__param__the_start) - 1;
};

uint16_t get_section_count(void)
{
//    return (uint16_t) param_section_count;
    return (&__section__the_end - &__section__the_start) - 1;
}

uint16_t get_param_handle(char* name)
{
    int index;
    char buff[32];

    struct param_info_s* pparam;

    for(index = 0; index < ((&__param__the_end - &__param__the_start) - 1 ); index++)
    {
        pparam = &param_info_base[index];
        strcpy(buff, pparam->name);
        if(strcmp(buff, name) == 0)
            return index;
    }

    return INVALID_PARAMETER_HANDLE;
}

const struct param_info_s* get_param(uint16_t handle)
{
    if(handle > (&__param__the_end - &__param__the_start) ) return NULL;
    return &param_info_base[handle];
}

parameter_union_t get_param_val(uint16_t handle);
uint16_t get_param_udb_type(uint16_t handle);



uint16_t get_section_handle(char* name);

const struct param_section_s* get_section(uint16_t handle)
{
    return &param_section_base[handle];
}

// Get the count of parameters contained in a given section
extern uint16_t get_section_params_count(uint16_t handle)
{
    uint16_t    count = 0;
    struct      param_section_s* psect;
    struct      param_info_s* pparam;
    char        buff[17];
    uint16_t    index;

    psect = &param_info_base[handle];

    for(index = 0; index < ((&__param__the_end - &__param__the_start) - 1 ); index++)
    {
        sprintf(buff, "%s_",  psect->name);

       if(strncmp(pparam->name, buff, strlen(buff)) == 0)
           count++;
    }

    return count;
}

