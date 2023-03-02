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

SGDK 1.80 (https://github.com/Stephane-D/SGDK/wiki/Setup-SGDK-with-CodeBlocks)

Code::Blocks 20.03

---

/out - Look for the latest wip roms here (*.bin).

	rom.bin - Latest test build.

	rom-pro.bin - Main version for Mega Everdrive Pro. Also should work in RetroArch (BlastEm core).

	rom-x7.bin - Stripped down version for Mega Everdrive X7. [wip, not tested]

	rom-blastem.bin - Version for standalone BlastEm emulator (modified rom header). [currently doesn't work]
	
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
![2022-09-03 12_43_57-RetroArch BlastEm 0 6 3-pre](https://user-images.githubusercontent.com/36992223/188265546-520a7bba-c9bd-4e40-8103-254bb4b3c89d.png)

Pattern editor
![2022-09-03 12_47_38-RetroArch BlastEm 0 6 3-pre](https://user-images.githubusercontent.com/36992223/188265556-c716742e-3b5f-4977-9dc9-709a10657c2c.png)

Instrument editor
![2022-09-03 12_50_25-RetroArch BlastEm 0 6 3-pre](https://user-images.githubusercontent.com/36992223/188265567-da0892f1-48e1-424a-98b6-b064d62e36fe.png)
