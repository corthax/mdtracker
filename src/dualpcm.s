*Z80ROM:         .incbin "D:/gamedev/sega/project/mdtracker/src/boot/Dual PCM - FlexEd.bin"
*Z80ROM_End:     .even
*                lea (Z80ROM).l,a0                       /* load Z80 ROM data */
*                lea (0xA00000).l,a1                     /* load Z80 RAM space address */
*                move.w #(Z80ROM_End-Z80ROM)-0x01,d1     /* set repeat times */
*stp:            move.w #0x0100,(0xA11100).l             /* request Z80 stop (ON) */
*                move.w #0x0100,(0xA11200).l             /* request Z80 reset (OFF) */
*                btst.b #0x00,(0xA11100).l               /* has the Z80 stopped yet? */
*                bne.s stp                               /* if not, branch */
*.LoadZ80:
*                move.b (a0)+,(a1)+                      /* copy Dual PCM to Z80 RAM */
*                dbf d1,.LoadZ80                         /* repeat til done */
*                lea (MuteSample).l,a0                   /* load mute sample address */
*                lea (0xA00C62).l,a1                     /* load Z80 RAM space where the pointer is to be stored */
*                move.b (a0)+,(a1)+                      /* copy pointer over into Z80 */
*                move.b (a0)+,(a1)+                      /* '' */
*                move.b (a0)+,(a1)+                      /* '' */
*                move.b (a0)+,(a1)+                      /* copy "reverse" pointer over into Z80 */
*                move.b (a0)+,(a1)+                      /* '' */
*                move.b (a0)+,(a1)+                      /* '' */
*                move.w #0x0000,(0xA11200).l             /* request Z80 reset (ON) */
*                moveq #0x7F,d1                          /* set repeat times */
*loop:           nop
*                dbf d1,loop                             /* no way of checking for reset, so a manual delay is necessary */
*                move.w #0x0000,(0xA11100).l             /* request Z80 stop (OFF) */
*                move.w #0x0100,(0xA11200).l             /* request Z80 reset (OFF) */

*                        align   0x1000
*                        dcb.b   0x180,0x00                      /* End marker */
*SWF_MuteSample:         dcb.b   0x8000-((0x18*0x10)*2),0x80     /* a large block of silent PCM data */
*SWF_MuteSample_Rev:     dcb.b   0x18*0x10,0x00                  /* End marker */
*SWF_S1_Kick:           incbin  "Samples\incswf\Sonic 1 Kick.swf"
*SWF_S1_Kick_Rev:       dcb.b   0x18*0x10,0x00
*SWF_S1_Snare:          incbin  "Samples\incswf\Sonic 1 Snare.swf"
*SWF_S1_Snare_Rev:      dcb.b   0x18*0x10,0x00
*SWF_S1_Timpani:        incbin  "Samples\incswf\Sonic 1 Timpani.swf"
*SWF_S1_Timpani_Rev:    dcb.b   0x18*0x10,0x00

*MuteSample:     dc.b ((SWF_MuteSample)&0xFF)
*                dc.b ((((SWF_MuteSample)>>0x08)&0x7F)|0x80)
*                dc.b (((SWF_MuteSample)&0x7F8000)>>0x0F)
*                dc.b ((SWF_MuteSample_Rev)&0xFF)
*                dc.b ((((SWF_MuteSample_Rev)>>0x08)&0x7F)|0x80)
*                dc.b (((SWF_MuteSample_Rev)&0x7F8000)>>0x0F)

*dcz80           macro Sample, SampleRev, SampleLoop, SampleLoopRev
*                dc.b ((Sample)&0xFF)
*                dc.b ((((Sample)>>0x08)&0x7F)|0x80)
*                dc.b (((Sample)&0x7F8000)>>0x0F)
*                dc.b (((SampleRev)-1)&0xFF)
*                dc.b (((((SampleRev)-1)>>0x08)&0x7F)|0x80)
*                dc.b ((((SampleRev)-1)&0x7F8000)>>0x0F)
*                dc.b ((SampleLoop)&0xFF)
*                dc.b ((((SampleLoop)>>0x08)&0x7F)|0x80)
*                dc.b (((SampleLoop)&0x7F8000)>>0x0F)
*                dc.b (((SampleLoopRev)-1)&0xFF)
*                dc.b (((((SampleLoopRev)-1)>>0x08)&0x7F)|0x80)
*                dc.b ((((SampleLoopRev)-1)&0x7F8000)>>0x0F)
*endm

*Sonic1Kick:     dcz80 SWF_S1_Kick, SWF_S1_Kick_Rev, SWF_MuteSample, SWF_MuteSample_Rev
*Sonic1Snare:    dcz80 SWF_S1_Snare, SWF_S1_Snare_Rev, SWF_MuteSample, SWF_MuteSample_Rev
*Sonic1Timpani:  dcz80 SWF_S1_Timpani, SWF_S1_Timpani_Rev, SWF_MuteSample, SWF_MuteSample_Rev

*                lea (Sonic1Kick).l,a0           /* load sample pointers */
*                lea (0xA00C69).l,a1             /* load PCM1 pointers */
*                lea (0xA0064E).l,a2             /* load PCM1 request switch */
*                move.w #0x0100,(0xA11100).l     /* request Z80 stop (ON) */
*                btst.b #0x00,(0xA11100).l       /* has the Z80 stopped yet? */
*                bne.s *-0x08                    /* if not, branch */
*                move.b (a0)+,(a1)+              /* set address of sample */
*                move.b (a0)+,(a1)+              /* '' */
*                move.b (a0)+,(a1)+              /* '' */
*                move.b (a0)+,(a1)+              /* set address of reverse sample */
*                move.b (a0)+,(a1)+              /* '' */
*                move.b (a0)+,(a1)+              /* '' */
*                move.b (a0)+,(a1)+              /* set address of loop sample */
*                move.b (a0)+,(a1)+              /* '' */
*                move.b (a0)+,(a1)+              /* '' */
*                move.b (a0)+,(a1)+              /* set address of loop reverse sample */
*                move.b (a0)+,(a1)+              /* '' */
*                move.b (a0)+,(a1)+              /* '' */
*                move.b #0b11011010,(a2)         /* set request */
*                move.w #0x0000,(0xA11100).l     /* request Z80 stop (OFF) */

*                lea (0xA0064E).l,a2         /* load PCM1 request switch */
*                move.w #0x0100,(0xA11100).l /* request Z80 stop (ON) */
*                btst.b #0x00,(0xA11100).l   /* has the Z80 stopped yet? */
*                bne.s *-0x08                /* if not, branch */
*                move.b #0b11011010,(a2)     /* set request */
*                move.w #0x0000,(0xA11100).l /* request Z80 stop (OFF) */
