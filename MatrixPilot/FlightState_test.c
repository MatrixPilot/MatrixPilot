#include <stdio.h>
#include "FlightState.h"

#ifdef FSM_TEST

int main(int argc, char *argv[])
{
	struct FlightState thisContext;
	int retcode = 0;
	char *result;

	if (argc < 2)
	{
		fprintf(stderr, "No string to check.\n");
		retcode = 2;
	}
	else if (argc > 2)
	{
		fprintf(stderr, "Only one argument is accepted.\n");
		retcode = 3;
	}
	else
	{
/*
		FlightState_Init(&thisContext);
		if (FlightState_CheckString(&thisContext, argv[1]) == 0)
		{
			result = "not acceptable";
			retcode = 1;
		}
		else
		{
			result = "acceptable";
		}
		printf("The string \"%s\" is %s\n", argv[1], result);
 */
 	}

	return retcode;
}

#endif
