
.set mcd_leds, 0xFF8000
.globl msu_drv
.globl irq4
/*---------------------------------------------------------------vectors table*/
	dc.l	0,RST
	dc.l	INT,INT,INT,INT,INT,INT,INT
	dc.l	INT,INT,INT,INT,INT,INT,INT,INT
	dc.l	INT,INT,INT,INT,INT,INT,INT,INT
	dc.l	IE1,IE2,IE3,IE4,IE5,IE6
/*----------------------------------------------------------------------------*/

RST:
    move.l  #0x80000, %a7
    move    #0x2000, %sr
    jmp     msu_drv
    jmp     RST

IE4:
    movem.l %d0-%d1/%a0-%a1,-(%sp)
    jsr irq4
    movem.l (%sp)+,%d0-%d1/%a0-%a1
    rte

IE1:
IE2:
IE3:
IE5:
IE6:
INT:
    rte

   
