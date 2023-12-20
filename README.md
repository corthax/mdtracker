# mdtracker
 Native music tracker for SEGA MEGA DRIVE / GENESIS / NOMAD
 
 Works on:
 
	BlastEm (RetroArch)
	
	BlastEm (Standalone 0.6.3-pre)
	
	Mega Everdrive Pro
	
 Partially:
 
	PicoDrive (RetroArch)
	
	- No CSM
	- No DAC panning
	- Wrong PSG noise pitch
	
---

SGDK (https://github.com/Stephane-D/SGDK/wiki/Setup-SGDK-with-CodeBlocks)

Code::Blocks 20.03

---

/out - Look for the latest wip roms here (*.bin).

	rom.bin - Latest test build.

	rom-pro.bin - Main version for Mega Everdrive Pro. Also should work in RetroArch (BlastEm core).

	rom-x7.bin - Stripped down version for Mega Everdrive X7. [wip]

	rom-blastem.bin - Version for standalone BlastEm emulator (modified rom header).
	
	/releases - All releases. Also the newest wip release preparation goes there.
	
	/cd - CD audio exapmle, not used. [using both msu-md driver and ssf2 mapper is not possible on MED PRO]

/inc - Copied SGDK headers. Fast and dirty way to make code completion in Code::Blocks work.

/res - Graphics, samples etc.

/src - Source code.

See "MD.Tracker Readme.txt" for help on commands etc.

---

Discord: https://discord.gg/CE22HNNk5y

Demo music (recorded from sega nomad + guitars + vocals): https://youtu.be/evPza2j4I88

---

Pattern matrix
![mtx](https://github.com/corthax/mdtracker/assets/36992223/33f80f19-ddb0-41cb-8bf0-a5939ea235f1)

Pattern editor
![ptn](https://github.com/corthax/mdtracker/assets/36992223/ce5a0937-9419-46b0-a068-4add7f041287)

Instrument editor
![inst](https://github.com/corthax/mdtracker/assets/36992223/aeece754-a6d9-40f4-b04a-656ac43c8ab5)

