#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>

/* Exercise 2.10 */
LEAF(tlb_out)
//1: j 1b
nop
	mfc0	k1,CP0_ENTRYHI		// store ENTRYHI in $k1
	mtc0	a0,CP0_ENTRYHI		// store argument in ENTRYHI
	nop
	tlbp						// check whether the virtual addr in ENTRYHI is in tlb
	nop
	nop
	nop
	nop
	mfc0	k0,CP0_INDEX		// check by the highest bit
	bltz	k0,NOFOUND			// check by the highset bit
	nop
	mtc0	zero,CP0_ENTRYHI	// clear ENTRYHI
	mtc0	zero,CP0_ENTRYLO0	// clear ENTRYLO
	nop
	tlbwi						// clear the found tlb term
NOFOUND:

	mtc0	k1,CP0_ENTRYHI		// restore the origin ENTRYHI
	
	j	ra
	nop
END(tlb_out)
