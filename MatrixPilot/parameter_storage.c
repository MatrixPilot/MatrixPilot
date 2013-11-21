
#include "defines.h"
#include "parameter_storage.h"
#include "parameter_scaling.h"

#include "../libUDB/events.h"
#include "../libUDB/timer.h"


#if (WIN == 1)
#include "../libFlashFS/minGlue.h"
#else
#include "../libFlashFS/minGlue-mdd.h"
#endif
#include <string.h>

uint16_t parstore_event_handle = INVALID_HANDLE;
uint16_t parstore_timer_handle = TIMER_INVALID_HANDLE;

uint16_t parstore_section_index;

static void parstore_initS(void);
static void parstore_waitingS(void);
static void parstore_start_loadingS(void);
static void parstore_loadingS(void);
static void parstore_start_savingS(void);
static void parstore_savingS(void);

void (*parstoreS)(void) = &parstore_initS;  // &parstore_waitingS; //

enum
{
    PARSTORE_REQUEST_NONE,
    PARSTORE_REQUEST_SAVE,
    PARSTORE_REQUEST_LOAD
} PARSTORE_REQUESTS;

uint16_t parstore_loadsave_flags = 0;

uint16_t parstore_status = PARSTORE_REQUEST_NONE;


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
    parstore_status = PARSTORE_REQUEST_NONE;
}

static void parstore_initS(void)
{
    parstoreS = &parstore_waitingS;
    parstore_status = PARSTORE_REQUEST_LOAD;
    timer_start(parstore_timer_handle, 1000, false, parstore_event_handle);
}

static void parstore_waitingS(void)
{
    switch(parstore_status)
    {
        case PARSTORE_REQUEST_NONE:
            parstoreS = &parstore_waitingS;
            timer_start(parstore_timer_handle, 500, false, parstore_event_handle);
            break;
        case PARSTORE_REQUEST_SAVE:
            parstore_start_savingS();
            break;
        case PARSTORE_REQUEST_LOAD:
            parstore_start_loadingS();
            break;
    }
}

static void parstore_start_loadingS(void)
{
    parstore_section_index = 0;

    parstoreS = &parstore_loadingS;
    timer_start(parstore_timer_handle, 10, false, parstore_event_handle);
}


static void parstore_loadingS(void)
{
    struct param_section_s*     psect;
    struct param_info_s*        pparam;
    mavlink_param_union_t       param;
    char                        paramstr[64];       // Line read buffer
    char *                      pch;                //
    INI_FILETYPE                file;
    uint16_t maxparms = get_param_count();
    uint16_t maxsecs = get_section_count();
    uint16_t paramcount = 0;
    uint16_t param_index;

    psect = get_section(parstore_section_index);
//    strcpy(filename, psect->name);
    sprintf(paramstr, "%s.PAR",  psect->name);
    ini_openread(paramstr, &file);

    // TODO add parameter storage checking of parameter sequence
    // TODO add parameter storage version checking

    // For each file in the line find the parameter and laod it.
    while(ini_read(paramstr, sizeof(paramstr), &file) != 0)
    {
        // Find the parameter name and parameter
        pch = strtok (paramstr," ");
        if( pch != NULL )
        {
            // find the parameter for the string and check it is valid
            param_index = get_param_handle(pch);
            if(param_index != INVALID_PARAMETER_HANDLE)
            {
                int16_t tempint;
                uint16_t tempuint;
                float tempfloat;
                pparam = get_param(param_index);

                // Find the parameter value and parse it
                pch = strtok (NULL, " ");
                if( pch != NULL )
                {
                    param.type = get_param_mavlink_type(pparam);
                    switch(param.type)
                    {
                        case MAVLINK_TYPE_UINT32_T:
                            sscanf(pch, "%d", &param.param_uint32);
                            if(param_scale_write(param, pparam) == true)
                                paramcount++;
                            break;
                        case MAVLINK_TYPE_INT32_T:
                            sscanf(pch, "%d", &tempint);
                            param.param_int32 = (int32_t) tempint;
                            if(param_scale_write(param, pparam) == true)
                                paramcount++;
                            break;
                        case MAVLINK_TYPE_FLOAT:
                            sscanf(pch, "%f", &param.param_float);
                            if(param_scale_write(param, pparam) == true)
                                paramcount++;
                           break;
                        case MAVLINK_TYPE_UINT16_T:
                        case MAVLINK_TYPE_INT16_T:
                        default:
                            break;
                    }

                }
            }
        }

    }

    ini_close(&file);

    // If the number of correctly reloaded parameters is correct then run the callback
    if(psect->ploadCallback != NULL)
           psect->ploadCallback( paramcount == get_section_params_count(parstore_section_index) );


    parstore_section_index++;

    // Come back in 100ms to do the next section, otherwise stop
    if(parstore_section_index < maxsecs)
        timer_start(parstore_timer_handle, 100, false, parstore_event_handle);
    else
    {
        parstore_status = PARSTORE_REQUEST_NONE;
        parstore_loadsave_flags = 0;
        parstoreS = &parstore_waitingS;
    }
}


static void parstore_start_savingS(void)
{
    parstore_section_index = 0;

    parstoreS = &parstore_savingS;
    timer_start(parstore_timer_handle, 10, false, parstore_event_handle);
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

    uint16_t parstore_param_index;
    psect = get_section(parstore_section_index);
//    strcpy(filename, psect->name);
    sprintf(paramstr, "%s.PAR",  psect->name);
    ini_openwrite(paramstr, &file);

    // Iterate through the parameter list looking for params belonging to the
    // group.  When one is found, save it to the file.
    while(parstore_param_index < maxparms)
    {
        pparam = get_param(parstore_param_index);

        // Compare name of section and the name of the parameter
        sprintf(paramstr, "%s_",  psect->name);
        
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
    {
        parstore_status = PARSTORE_REQUEST_NONE;
        parstore_loadsave_flags = 0;
        parstoreS = &parstore_waitingS;
    }
}

// save parameters to storage
void save_parameters(uint16_t flags)
{
    if(parstore_status == PARSTORE_REQUEST_NONE)
    {
        parstore_loadsave_flags = flags;
        parstore_status = PARSTORE_REQUEST_SAVE;
    }
}

// load parameters from storage
void load_parameters(uint16_t flags)
{
    if(parstore_status == PARSTORE_REQUEST_NONE)
    {
        parstore_loadsave_flags = flags;
        parstore_status = PARSTORE_REQUEST_LOAD;
    }
}