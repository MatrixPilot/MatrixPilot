#ifdef __dsPIC33FJ256GP710A__
#include "p33FJ256GP710A.h"
#endif

#ifdef __dsPIC33EP512MU810__
#include "p33EP512MU810.h"
#endif

#ifdef __dsPIC33FJ128MC708__
#include "p33FJ128MC708.h"
#endif

// Declaration for TestSubsets
extern int TestSubset_1(void);
extern int TestSubset_2(void);
extern int TestSubset_3(void);
extern int TestSubset_4(void);
extern int TestSubset_5(void);
extern int TestSubset_6(void);
#if defined(__dsPIC33F__) || defined(__dsPIC33E__)
extern int TestSubset_7(void);   // These declarations to be included when
extern int TestSubset_8(void);   // dsPIC33F device is present.
#endif

/* ----------------------------------------------------------------------- */
/* Define arrays to hold the results as well as the expected test results. */
/* NOTE: Some ExpectedResults[] values will vary for other device variants */
/* ----------------------------------------------------------------------- */
#if defined(__dsPIC33FJ256GP710A__)
   int TestResults[8] = {0, 0, 0, 0, 0, 0, 0, 0};
   int ExpectedResults[8] = {0x694D, 0x6003, 0xC2A7, 0x00F8, 0x1BD2, 0xAE40, 0xE340, 0x1D7D};
#elif defined(__PIC24HJ256GP610A__)
   int TestResults[6] = {0, 0, 0, 0, 0, 0};
   int ExpectedResults[6] = {0x694D, 0x5F7F, 0xC2A7, 0x00F8, 0x1BD2, 0xAE40};
#elif defined(__dsPIC33EP512MU810__)
   int TestResults[8] = {0, 0, 0, 0, 0, 0, 0, 0};
   int ExpectedResults[8] = {0x694D, 0x7476, 0xC2AB, 0x00F8, 0x1BD2, 0xAE40, 0x7222, 0x1D7D};
#elif defined(__PIC24EP512GU810__)
   int TestResults[6] = {0, 0, 0, 0, 0, 0};
   int ExpectedResults[6] = {0x694D, 0x743A, 0xC2AB, 0x00F8, 0x1BD2, 0xAE40};
#elif defined(__dsPIC33EP64MC506__)
   int TestResults[8] = {0, 0, 0, 0, 0, 0, 0, 0};
   int ExpectedResults[8] = {0x694D, 0xCE7E, 0xC2AB, 0x00F8, 0x1BD2, 0xAE40, 0xABA4, 0x1D7D};
#endif

// ****************************************************************************
// If the device selected is PIC24H/PIC24E   - Subset_Called vary from 1 to 6.
// If the device selected is dsPIC33F/dsPIC33E - Subset_Called vary from 1 to 8.
// ****************************************************************************
int (*CPUTests[])(void) = { TestSubset_1,
							TestSubset_2,
							TestSubset_3,
							TestSubset_4,
							TestSubset_5,
							TestSubset_6,
#if defined(__dsPIC33F__) || defined(__dsPIC33E__)
							TestSubset_7,
							TestSubset_8
#endif
};

int cputest(void)
{
	int i;
	int result = 0;

	for (i = 0; i < (sizeof(CPUTests) / sizeof(CPUTests[0])); i++) {
		if (ExpectedResults[i] != CPUTests[i]()) {
			result |= (1 << i);
		}
	}
	return result;
}
