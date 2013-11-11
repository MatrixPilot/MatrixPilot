#include "../libUDB/libUDB.h"

/**
 * Array of static parameter info.
 */

PARAM_DEFINE_END();
PARAM_DEFINE_INT32(ZIP_BAG, 0);
PARAM_DEFINE_START();

extern char __param_start, __param_end;
static const struct param_info_s	*param_info_base = &(((struct param_info_s *) &__param__the_start)[1]);
static const struct param_info_s	*param_info_limit = (struct param_info_s *) &__param__the_end;
#define	param_info_count		((unsigned)(param_info_limit - param_info_base))

#define param_count param_info_count/sizeof(param_info_s)
