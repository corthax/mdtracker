# msu-md
![msu logo](https://github.com/krikzz/msu-md/blob/master/msu.png)

### MegaCD driver for msu-like interfacing with CD hardware
for MegaDrive/Genesis

---

#### control registers

| Address  | Type          | Details                            |
| :------- | :------------ | :--------------------------------- |
| 0xA12010 | command       | see section **commands**           |
| 0xA12011 | argument      | argument for each command          |
| 0xA1201F | command clock | increment it for command execution |
| 0xA12020 | mcd status    | 0-ready, 1-init, 2-cmd busy        |
 	
---

#### commands

| Code | Type      | Arguments                                                                                                      | Details                                               |
| :--- | :-------- | :------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------- |
| 0x11 | PLAY      | decimal no. of track (1-99)                                                                                    | playback will be stopped in the end of track          |
| 0x12 | PLAY LOOP | decimal no. of track (1-99)                                                                                    | playback will restart the track when end is reached   |
| 0x13 | PAUSE     | vol fading time.<br>1/75 of sec<br>(75 equal to 1 sec) instant stop if 0                                       | pause playback                                        |
| 0x14 | RESUME    | none                                                                                                           | resume playback                                       |
| 0x15 | VOL       | volume 0-255                                                                                                   | set cdda volume                                       |
| 0x16 | NOSEEK    | 0-on(default state),<br>1-off(no seek delays)                                                                  | seek time emulation switch                            |
| 0x1A | PLAYOF    | #1 = decimal no. of track (1-99)<br>#2 = offset in sectors from the start of the track to apply when looping | play cdda track and loop from specified sector offset |

---

#### usage

* Include msu-drv.bin in your binary and call it as function for initialization. 
* Function will return 0 if driver loaded successfully or 1 if MCD hardware not detected.
  * Driver ready for commands processing when 0xA12020 sets to 0

--- 

#### repository contents

| content       | Details                |
| :------------ | :--------------------- |
| msu-drv.bin   | compiled driver binary |
| msu-md-drv    | driver sources         |
| msu-md-sample | driver usage sample    |

---

#### Mega Everdrive Pro
[support files and firmware download](http://krikzz.com/pub/support/mega-everdrive/pro-series/) | [buy](https://krikzz.com/store/home/59-mega-everdrive-pro.html)

* Cyclone IV FPGA
* 16MB PSRAM and 1MB SRAM memory
* High quality 6-layers PCB with hard gold surface finish
* Supported ROM formats: Genesis, 32X[1], Mega-CD, Master System, NES[2]
* Ram cart for Mega-CD
* CD bios swap function turns off region locks for Mega-CD games
* Hardware MegaKey turns off region locks for Genesis games
* YM2413 core for Master System games
* SVP core support
* Pier Solar mapper support
* In-game menu. You can save game or return to system menu without physical reset
* In-game menu supported for Genesis and Master System modes
* Full EEPROM saves support
* Real time clock for logging date and time of saves
* ARM based 32bit I/O co-processor for SD and USB operations acceleration
* Up to 1024 files per folder or unlimited if file sorting is disabled
* Multi slot save states. 100 save states slots for each game
* Instant ROM loading
* USB port for development and system update without removing SD card
* Battery voltage monitoring. Battery can be replaced in time if it runs dry
* Game Genie cheats support

_[1] 32X games require 32X addon to be attached.<br>
 [2] NES core has limited compatibility. Refer to user manual for details.<br>
NES core currently supported only for NTSC systems.<br>
For Mega-SG "Zero lag" option should be turned off for NES games (accurate Genesis timings required)._