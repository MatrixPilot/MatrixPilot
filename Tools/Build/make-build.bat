@echo off
@rem pyProjectCreator.py -t UDB4 --dir="MatrixPilot" --dir="libUDB" --dir="libDCM" --dir="libFlashFS" --dir="libVectorMatrix" --dir="MAVLink" --dir="Microchip"
@rem pyProjectCreator.py -t UDB5 --dir="MatrixPilot" --dir="libUDB" --dir="libDCM" --dir="libFlashFS" --dir="libVectorMatrix" --dir="MAVLink" --dir="Microchip"
@rem pyProjectCreator.py -t AUAV3 --dir="MatrixPilot" --dir="libUDB" --dir="libDCM" --dir="libFlashFS" --dir="libVectorMatrix" --dir="MAVLink" --dir="Microchip"

pyProjectCreator.py -t UDB4  -o ..\..\build -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip
pyProjectCreator.py -t UDB5  -o ..\..\build -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip
pyProjectCreator.py -t AUAV3 -o ..\..\build -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip

copy build-auav3.bat ..\..\build
copy build-silsim.bat ..\..\build

@rem pyProj.py -t AUAV3 --dir="MatrixPilot" --dir="libDCM" --dir="MAVLink" 
@rem pyProj.py -t AUAV3 --dir="libUDB" --dir="libFlashFS" --dir="libVectorMatrix" --dir="Microchip"
