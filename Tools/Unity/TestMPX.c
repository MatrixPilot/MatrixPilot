/* ==========================================
    Unity Project - A Test Framework for C
    Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
========================================== */

#include <setjmp.h>
#include "unity.h"

#include "../../libUDB/udbTypes.h"
#include "../../libDCM/dcmTypes.h"

#include "../../MatrixPilot/defines.h"
#include "../../libUDB/events.h"
#include "../../MatrixPilot/mode_switch.h"

// Dividing by these constants produces +/- infinity.
// The rationale is given in UnityAssertFloatIsInf's body.
static const _UF f_zero = 0.0f;
#ifndef UNITY_EXCLUDE_DOUBLE
static const _UD d_zero = 0.0;
#endif

#define EXPECT_ABORT_BEGIN \
    if (TEST_PROTECT())    \
    {
    
#define VERIFY_FAILS_END                                                       \
    }                                                                          \
    Unity.CurrentTestFailed = (Unity.CurrentTestFailed == 1) ? 0 : 1;          \
    if (Unity.CurrentTestFailed == 1) {                                        \
      SetToOneMeanWeAlreadyCheckedThisGuy = 1;                                 \
      UnityPrint("[[[[ Previous Test Should Have Failed But Did Not ]]]]");    \
      UNITY_OUTPUT_CHAR('\n');                                                 \
    }
    
#define VERIFY_IGNORES_END                                                     \
    }                                                                          \
    Unity.CurrentTestFailed = (Unity.CurrentTestIgnored == 1) ? 0 : 1;         \
    Unity.CurrentTestIgnored = 0;                                              \
    if (Unity.CurrentTestFailed == 1) {                                        \
      SetToOneMeanWeAlreadyCheckedThisGuy = 1;                                 \
      UnityPrint("[[[[ Previous Test Should Have Ignored But Did Not ]]]]");   \
      UNITY_OUTPUT_CHAR('\n');                                                 \
    }

int SetToOneToFailInTearDown;
int SetToOneMeanWeAlreadyCheckedThisGuy;

void setUp(void)
{
  SetToOneToFailInTearDown = 0;
  SetToOneMeanWeAlreadyCheckedThisGuy = 0;
}

void tearDown(void)
{
  if (SetToOneToFailInTearDown == 1)
    TEST_FAIL_MESSAGE("<= Failed in tearDown");
  if ((SetToOneMeanWeAlreadyCheckedThisGuy == 0) && (Unity.CurrentTestFailed > 0))
  {
    UnityPrint("[[[[ Previous Test Should Have Passed But Did Not ]]]]");
    UNITY_OUTPUT_CHAR('\n');
  }
}

void testUnitySizeInitializationReminder(void)
{
    /* This test ensures that sizeof(struct _Unity) doesn't change. If this
     * test breaks, go look at the initialization of the Unity global variable
     * in unity.c and make sure we're filling in the proper fields. */
    char * message = "Unexpected size for _Unity struct. Please check that "
                     "the initialization of the Unity symbol in unity.c is "
                     "still correct.";

    /* Define a structure with all the same fields as `struct _Unity`. */
    struct {
        const char* TestFile;
        const char* CurrentTestName;
        UNITY_LINE_TYPE CurrentTestLineNumber;
        UNITY_COUNTER_TYPE NumberOfTests;
        UNITY_COUNTER_TYPE TestFailures;
        UNITY_COUNTER_TYPE TestIgnores;
        UNITY_COUNTER_TYPE CurrentTestFailed;
        UNITY_COUNTER_TYPE CurrentTestIgnored;
        jmp_buf AbortFrame;
    } _Expected_Unity;

    /* Compare our fake structure's size to the actual structure's size. They
     * should be the same.
     *
     * This accounts for alignment, padding, and packing issues that might come
     * up between different architectures. */
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(_Expected_Unity), sizeof(Unity), message);
}

static event_callback1_value = 0;
static event_callback2_value = 0;

void event_callback1(void)
{
	event_callback1_value = 0x3434;
}

void event_callback2(void)
{
	event_callback2_value = 42;
}

void test_events(void)
{
	int i;
	uint16_t hEvents[MAX_EVENTS];

	for (i = 0; i < MAX_EVENTS; i++) {
		hEvents[i] = register_event((i % 2) ? event_callback1 : event_callback2);
		TEST_ASSERT_NOT_EQUAL(INVALID_HANDLE, hEvents[i]);
	}
	trigger_event(hEvents[0]);
	trigger_event(hEvents[MAX_EVENTS-1]);
	process_queued_events();
	
	TEST_ASSERT_EQUAL(0x3434, event_callback1_value);
	TEST_ASSERT_EQUAL(42,     event_callback2_value);
	TEST_ASSERT_EQUAL(INVALID_HANDLE, register_event(event_callback1));
}

void test_flight_mode_switch_check_set(void)
{
/*
	udb_flags._.radio_on = 0;
	flight_mode_switch_check_set();
	TEST_ASSERT_EQUAL(0, flight_mode_switch_manual());
	TEST_ASSERT_EQUAL(0, flight_mode_switch_stabilize());
	TEST_ASSERT_EQUAL(1, flight_mode_switch_waypoints());

	udb_flags._.radio_on = 1;
#if (MODE_SWITCH_TWO_POSITION == 1)

	udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] = MODE_SWITCH_THRESHOLD_LOW;
	flight_mode_switch_2pos_poll(); // this is called at 40 hertz
//	request_autopilot_mode = FLIGHT_MODE_SWITCH_AUTONOMOUS;
	flight_mode_switch_check_set();
	TEST_ASSERT_EQUAL(0, flight_mode_switch_manual());
	TEST_ASSERT_EQUAL(0, flight_mode_switch_stabilize());
	TEST_ASSERT_EQUAL(1, flight_mode_switch_waypoints());

	udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] = MODE_SWITCH_THRESHOLD_LOW + 1;
	flight_mode_switch_2pos_poll();
	flight_mode_switch_2pos_poll();
//	request_autopilot_mode = FLIGHT_MODE_SWITCH_STABILIZED;
	flight_mode_switch_check_set();
	TEST_ASSERT_EQUAL(0, flight_mode_switch_manual());
	TEST_ASSERT_EQUAL(1, flight_mode_switch_stabilize());
	TEST_ASSERT_EQUAL(0, flight_mode_switch_waypoints());

	udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] = MODE_SWITCH_THRESHOLD_LOW - 1;
	flight_mode_switch_2pos_poll();
	flight_mode_switch_2pos_poll();
//	request_autopilot_mode = FLIGHT_MODE_SWITCH_MANUAL;
	flight_mode_switch_check_set();
	TEST_ASSERT_EQUAL(1, flight_mode_switch_manual());
	TEST_ASSERT_EQUAL(0, flight_mode_switch_stabilize());
	TEST_ASSERT_EQUAL(0, flight_mode_switch_waypoints());
#else
#endif
 */
 }

