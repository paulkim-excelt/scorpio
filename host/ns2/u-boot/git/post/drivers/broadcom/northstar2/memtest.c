/**********************************************************************
 *
 * Filename:    memtest.c
 * 
 * Description: General-purpose memory testing functions.
 *
 * Notes:       This software can be easily ported to systems with
 *              different data bus widths by redefining 'datum'.
 *
 * 
 * Copyright (c) 1998 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/


#include "memtest.h"
#include <post.h>


/**********************************************************************
 *
 * Function:    memTestDataBus()
 *
 * Description: Test the data bus wiring in a memory region by
 *              performing a walking 1's test at a fixed address
 *              within that region.  The address (and hence the
 *              memory region) is selected by the caller.
 *
 * Notes:       
 *
 * Returns:     0 if the test succeeds.  
 *              A non-zero result is the first pattern that failed.
 *
 **********************************************************************/
datum
memTestDataBus(volatile datum * address)
{
    datum pattern;

	post_log("Memory Data Bus(walking 1's) test at address: 0x%lx\n", address);

    /*
     * Perform a walking 1's test at the given address.
     */
    for (pattern = 1; pattern != 0; pattern <<= 1)
    {
        /*
         * Write the test pattern.
         */
        *address = pattern;

        /*
         * Read it back (immediately is okay for this test).
         */
        if (*address != pattern) 
         {
			post_log("pattern 0x%lx failed at 0x%lx\n", pattern, address);
            return (pattern);
        } else {
			//post_log("pattern 0x%lx passed at 0x%lx\n", pattern, address);
		}
    }

	post_log("Memory Data Bus walking 1's test Passed\n");
    return (0);

}   /* memTestDataBus() */


/**********************************************************************
 *
 * Function:    memTestAddressBus()
 *
 * Description: Test the address bus wiring in a memory region by
 *              performing a walking 1's test on the relevant bits
 *              of the address and checking for aliasing. This test
 *              will find single-bit address failures such as stuck
 *              -high, stuck-low, and shorted pins.  The base address
 *              and size of the region are selected by the caller.
 *
 * Notes:       For best results, the selected base address should
 *              have enough LSB 0's to guarantee single address bit
 *              changes.  For example, to test a 64-Kbyte region, 
 *              select a base address on a 64-Kbyte boundary.  Also, 
 *              select the region size as a power-of-two--if at all 
 *              possible.
 *
 * Returns:     NULL if the test succeeds.  
 *              A non-zero result is the first address at which an
 *              aliasing problem was uncovered.  By examining the
 *              contents of memory, it may be possible to gather
 *              additional information about the problem.
 *
 **********************************************************************/
datum * 
memTestAddressBus(volatile datum * baseAddress, unsigned long nBytes)
{
    unsigned long addressMask = (nBytes/sizeof(datum) - 1);
    unsigned long offset;
    unsigned long testOffset;

    datum pattern     = (datum) 0xAAAAAAAAAAAAAAAA;
    datum antipattern = (datum) 0x5555555555555555;

	post_log("Memory Address Bus(walking 1's) test, start: 0x%lx, %ld bytes\n", baseAddress, nBytes);

    /*
     * Write the default pattern at each of the power-of-two offsets.
     */
    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        baseAddress[offset] = pattern;
		//post_log("Written pattern:0x%lx at address:0x%lx\n", pattern, &baseAddress[offset]);
    }

    /* 
     * Check for address bits stuck high.
     */
    testOffset = 0;
    baseAddress[testOffset] = antipattern;

	post_log("Memory Address Bus Test for address bits stuck high...");
    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        if (baseAddress[offset] != pattern)
        {
			post_log("Address Bus Test failed at 0x%lx\n", &baseAddress[offset]);
            return ((datum *) &baseAddress[offset]);
        }
		else
		{
			//post_log("Address Bus Test passed at 0x%lx\n", &baseAddress[offset]);
		}
    }
	post_log("...Passed\n");

    baseAddress[testOffset] = pattern;

    /*
     * Check for address bits stuck low or shorted.
     */
	post_log("Memory Address Bus Test for address bits stuck low...");
    for (testOffset = 1; (testOffset & addressMask) != 0; testOffset <<= 1)
    {
        baseAddress[testOffset] = antipattern;
		//post_log("Written antipattern:0x%lx at address:0x%lx\n", antipattern, &baseAddress[offset]);

		if (baseAddress[0] != pattern)
		{
			post_log("Address Bus Test failed at 0x%lx\n", &baseAddress[0]);
			return ((datum *) &baseAddress[testOffset]);
		}

        for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
        {
            if ((baseAddress[offset] != pattern) && (offset != testOffset))
            {
				post_log("Address Bus Test failed at 0x%lx\n", &baseAddress[offset]);
                return ((datum *) &baseAddress[testOffset]);
            }
        }

        baseAddress[testOffset] = pattern;
    }
	post_log("...Passed\n");

	post_log("Memory Address Bus walking 1's test Passed\n");
    return (NULL);

}   /* memTestAddressBus() */


/**********************************************************************
 *
 * Function:    memTestDevice()
 *
 * Description: Test the integrity of a physical memory device by
 *              performing an increment/decrement test over the
 *              entire region.  In the process every storage bit 
 *              in the device is tested as a zero and a one.  The
 *              base address and the size of the region are
 *              selected by the caller.
 *
 * Notes:       
 *
 * Returns:     NULL if the test succeeds.
 *
 *              A non-zero result is the first address at which an
 *              incorrect value was read back.  By examining the
 *              contents of memory, it may be possible to gather
 *              additional information about the problem.
 *
 **********************************************************************/
datum * 
memTestDevice(volatile datum * baseAddress, unsigned long nBytes)	
{
    unsigned long offset;
    unsigned long nWords = nBytes / sizeof(datum);

    datum pattern;
    datum antipattern;

	post_log("Memory Device Test increment pattern, start: 0x%lx, %ld bytes\n", baseAddress, nBytes);

    /*
     * Fill memory with a known pattern.
     */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        baseAddress[offset] = pattern;
    }

    /*
     * Check each location and invert it for the second pass.
     */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        if (baseAddress[offset] != pattern)
        {
			post_log("Memory test failed at: 0x%lx\n", &baseAddress[offset]);
            return ((datum *) &baseAddress[offset]);
        }

        antipattern = ~pattern;
        baseAddress[offset] = antipattern;
    }

    /*
     * Check each location for the inverted pattern and zero it.
     */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        antipattern = ~pattern;
        if (baseAddress[offset] != antipattern)
        {
			post_log("Memory test failed at: 0x%lx\n", &baseAddress[offset]);
            return ((datum *) &baseAddress[offset]);
        }
    }

	post_log("Memory Device incremental pattern test Passed\n");

    return (NULL);

}   /* memTestDevice() */
