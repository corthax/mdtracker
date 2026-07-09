
.set mcd_wram, 0x420000
.set mcd_crtl, 0xA12001
.set mcd_mewp, 0xA12002
.set mcd_mmod, 0xA12003
.set mcd_comf, 0xA1200F

.set ctrl_rst, 0x01
.set ctrl_brq, 0x02


start:
*reset mc
    move.w  #1, %d0             /*1 in d0 if error*/
    cmp.l   #0x53454741, 0x400100
    bne     exit
    move.b  #ctrl_brq, mcd_crtl
bus_req:
    btst    #1, mcd_crtl
    
*copy sub code
    move.b  #0, mcd_mmod        /*set wram bank0*/
    move.l  #(sub_code - copy - 2), %a0
    move.l  #mcd_wram, %a1
    move.w  #(sub_code_end - sub_code)/2, %d0
copy:
    move.w (%pc, %a0), (%a1)+
    add.w   #2, %a0
    dbra    %d0, copy
    

*run mcd
    move.b  #0, mcd_comf
    move.b  #ctrl_rst, mcd_crtl
wait_mcd:
    move.b  mcd_crtl, %d0
    and.b   #ctrl_rst, %d0
    beq     wait_mcd
    
    move.b  #0, mcd_mewp        /*turn off wram protection*/

    move.w  #0, %d0             /*0 in d0 if ok*/
exit:
    rts

sub_code:
.incbin "sub.bin"
sub_code_end:
