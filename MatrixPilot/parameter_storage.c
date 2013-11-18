
#include "defines.h"
#include "parameter_storage.h"
#include "parameter_scaling.h"

#include "../libUDB/events.h"
#include "../libUDB/timer.h"


#include "../libFlashFS/minGlue-mdd.h"
#include <string.h>

uint16_t parstore_event_handle = INVALID_HANDLE;
uint16_t parstore_timer_handle = TIMER_INVALID_HANDLE;

uint16_t parstore_section_index;
uint16_t parstore_param_index;

static void parstore_initS(void);
static void parstore_waitingS(void);
static void parstore_start_loadingS(void);
static void parstore_loadingS(void);
static void parstore_start_savingS(void);
static void parstore_savingS(void);

void (*parstoreS)(void) = &parstore_initS;  // &parstore_waitingS; //


void parstore_callback(void)
{
    parstoreS();
}


// Initialize storage
void init_parameter_storage(void)
{
    parstore_event_handle = register_event_p(&parstore_callback, EVENT_PRIORITY_LOW);
    parstore_timer_handle = timer_register();

    // Non repeating timer to start of initialisation
    timer_start(parstore_timer_handle, 1000, false, parstore_event_handle);
}

static void parstore_initS(void)
{
    parstoreS = &parstore_waitingS;
    timer_start(parstore_timer_handle, 1000, false, parstore_event_handle);
}

static void parstore_waitingS(void)
{
    timer_start(parstore_timer_handle, 1000, false, parstore_event_handle);
}

static void parstore_start_loadingS(void)
{
}

static void parstore_loadingS(void)
{
}

static void parstore_start_savingS(void)
{

    parstore_section_index = 0;
    parstore_param_index = 0;

    parstoreS = &parstore_savingS;
    timer_start(parstore_timer_handle, 1000, false, parstore_event_handle);
}

static void parstore_savingS(void)
{
    struct param_section_s*    psect;
    struct param_info_s*       pparam;
    mavlink_param_union_t       param;
    char                paramstr[64];
    INI_FILETYPE        file;
    uint16_t maxparms = get_param_count();
    uint16_t maxsecs = get_section_count();

    parstore_param_index = 0;
    psect = get_section(parstore_section_index);
//    strcpy(filename, psect->name);
    sprintf(paramstr, "%s.PAR",  psect->name);
    ini_openwrite(paramstr, &file);

    while(parstore_param_index < maxparms)
    {
        pparam = get_param(parstore_param_index);

        // Compare name of section and the name of the parameter
        sprintf(paramstr, "%s_",  psect->name);
//        int8_t str_len = strlen(paramstr);
        
        if ( strncmp(pparam->name, paramstr, strlen(paramstr) ) == 0)
        {

            strcpy(paramstr, pparam->name);
            ini_write(paramstr, &file);
            ini_write(" ", &file);
            param = param_scale_read(pparam);

            switch(param.type)
            {
                case MAVLINK_TYPE_UINT32_T:
                    sprintf(paramstr, "%u", param.param_uint32);
                    break;
                case MAVLINK_TYPE_INT32_T:
                    sprintf(paramstr, "%d", param.param_int32);
                    break;
                case MAVLINK_TYPE_FLOAT:
                    sprintf(paramstr, "%f", param.param_float);
                   break;
                case MAVLINK_TYPE_UINT16_T:
                case MAVLINK_TYPE_INT16_T:
                default:
                    strcpy(paramstr, "Unsupported type");
                    break;
            }

            ini_write(paramstr, &file);
            ini_write("\r\n", &file);
        }
        parstore_param_index++;
    }

    ini_close(&file);

    parstore_section_index++;

    // Come back in 100ms to do the next section, otherwise stop
    if(parstore_section_index < maxsecs)
        timer_start(parstore_timer_handle, 100, false, parstore_event_handle);
    else
        parstoreS = &parstore_waitingS;
}

// save parameters to storage
void save_parameters(uint16_t flags)
{

}

// load parameters from storage
void load_parameters(uint16_t flags)
{
}