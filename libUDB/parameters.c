#include "../libUDB/libUDB.h"
//#include "../MatrixPilot/parameter_table.h"

/**
 * Array of static parameter info.
 */

unsigned int zip_bag;
fractional bluff;

PARAM_DEFINE_START();
PARAM_DEFINE_END();
PARAM_DEFINE_INT(MISC_ZIP_BAG, &zip_bag, 0, RMAX, false);
PARAM_DEFINE_Q14(MISC_BLUFF, &bluff, -1.0, 1.0, false);

PARAM_SECTION_START();
PARAM_SECTION_END();

extern char __param_start, __param_end;
static const struct param_info_s	*param_info_base = &(((struct param_info_s *) &__param__the_start)[1]);
static const struct param_info_s	*param_info_limit = (struct param_info_s *) &__param__the_end;
#define	param_info_count		((unsigned)(param_info_limit - param_info_base))
#define param_count (((uint32_t) param_info_count/sizeof(param_info_s)) - 1)


static const struct param_section_s	*param_section_base = &(((struct param_section_s *) &__section__the_start)[1]);
static const struct param_section_s	*param_section_limit = (struct param_section_s *) &__section__the_end;
#define	param_section_count		((unsigned)(param_section_limit - param_section_base))
#define section_count (((uint32_t) param_section_count/sizeof(param_section_s)) - 1)


inline uint16_t get_param_count(void) {return param_count;};
inline uint16_t get_section_count(void) {return section_count;};


uint16_t get_param_handle(char* name)
{
    int index;
    param_info_s* pparam;

    for(index = 0, index < param_count; index++;)
    {
        pparam = &param_info_base[index];
        if(strcmp(pparam->name, name) == 0)
            return index;
    }

    return INVALID_PARAMETER_HANDLE;
}

const struct param_info_s* get_param(uint16_t handle)
{
    return &param_info_base[handle];
}

param_union_t get_param_val(uint16_t handle);
uint16_t get_param_udb_type(uint16_t handle);
uint16_t get_param_mavlink_type(uint16_t handle);

uint16_t get_section_handle(char* name);
const struct param_section_s* get_section(uint16_t handle);
