GROUP(libc.a libg.a libnosys.a libm.a)

/* Specify the memory areas */
MEMORY
{
  /* ITCM full size is 480KB, as bottom 64KB is used by BL.
   * So, FW should utilise only 416KB (480KB - 64KB) */
  ITCM (xw)     : ORIGIN = 0x00008000, LENGTH = 416K
  DTCM (rw)     : ORIGIN = 0x20000000, LENGTH = 240K
  IPC (rw)      : ORIGIN = 0x2003C000, LENGTH = 16K
  SCRATCHMEM (xrw)  : ORIGIN = 0x64000000, LENGTH = 40K
}
