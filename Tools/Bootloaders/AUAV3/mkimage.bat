@echo off
@rem python hex2src.py --pad=FF --range=FF8000:FFFFFF "AUAV3 Bootloader.hex" > bootimage.s
python hex2src.py --pad=FF --range=FF8000:FFFFFF "AUAV3 Bootloader.hex" "bootimage.s"
