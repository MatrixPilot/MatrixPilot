# MatrixPilot 

[Travis CI](https://travis-ci.org/MatrixPilot/MatrixPilot) ![Travis CI](https://api.travis-ci.org/MatrixPilot/MatrixPilot.svg?branch=master)

## Firmware for Bill Premerlani's IMU based UAV Dev Board and its successors

Welcome to the MatrixPilot codebase. Four board types are supported:-
* the [UDB4](https://www.sparkfun.com/products/retired/11115) (Production retired at SparkFun)
* the [UDB5](https://www.sparkfun.com/products/retired/11703) (Production retired at SparkFun)
* the UDB5 project in software also supports the [UDB5mini](https://octopilot.com/) currently available from Octopilot Electronics
* the AUAV3 from Arsov RC Technology. (Production retired).

Projects for the boards are provided for the MPLAB-X Integrated Development Environment (IDE) and XC16 compiler. From October 2016, the master branch no longer supports the legacy MPLAB IDE.

There are 4 projects in the repository. They are listed here in order of increasing complexity:-

* ./Tools/FlashOSD updates the character set of the native On Screen Display (OSD; additional OSD board required). 
* ./Tools/LedTest is used to test hardware including the accelerometers, gyros, Leds, and PWM outputs.It is factory installed for 3 boards allowing users to test their boards on delivery.
* ./RollPitchYaw creates an Inertial Measurement Unit (IMU). It  enables higher level testing of the IMU algorithms, firmware and hardware all combined together.
* ./MatrixPilot integrates the IMU with a full Autopilot for a plane, providing Fly By Wire control, Autonomous flight, Return to Landing (RTL), and Manual overide. 

There are two types of simulation avaiable:-
* Software in the Loop (SIL) is provided in ./Tools/MatrixPilot-SIL. The autopilot is simulated as a software process in a host computer (Unix, MacOS, Windows) and that is then, in turn, connected to fly an aircraft in the X-Plane 10 flight simulator.
* Hardware in the Loop (HIL) connects a physical board running MatrixPilot to a simulated aircraft in X-Plane 10. It is enabled for any board by setting a parameter in ./Config/options.h when compiling MatrixPilot. 

./Tools/HilSim provides the software plugin for X-Plane10 to link to the SIL or HIL versions of MatrixPilot. 

./Tools/flight_analyzer provides a python program that allows analysis and display of flights with the help of GoogleEarth, after post-processing with our tool 'flan'.

For developers, there is also a makefile based build system (build-all.bat) and a project generator (Tools/Build/proj_gen.bat), which both Windows and *nix compatible.

For more info about how to configure and use this autopilot firmware, go to the Wiki at:

   https://github.com/MatrixPilot/MatrixPilot/wiki

and see the mailing list at:

   https://groups.google.com/forum/#!forum/uavdevboard
