flan.py (which stands for Flight Analyzer)is  UAV Telemetry Analyzer for the
UAV DevBoard and it's assocated firmware called MatrixPilot.

It reads output from a telemetry file - and turns it into KML that
allows a flight to be analyzed using Google Earth (GE).

To see a picture of the possibilities - take a look at
http://picasaweb.google.co.uk/lh/photo/nfPkZ0y0-jWEBFdBulif_w?feat=directlink

Or watch this video for a short tutorial:
http://www.vimeo.com/12028630

GETTING STARTED:
Using MatrixPilot  you must first configure
options.h to send the correct format of telemetry out of the serial port.

In the options.h file, find the line that says :-
#define SERIAL_OUTPUT_FORMAT	SERIAL_NONE

and set it to :-
#define SERIAL_OUTPUT_FORMAT	SERIAL_UDB

Start up your plane and capture the telemetry into a file. (setting up 
wireless serial communications, or wired serial communications is beyond the scope
of this document). 

For this example we will capture telemetry to flight15.txt
( In Windows I use Accessories / Communications / HyperTerminal,
In hyper terminal I do "Transfer / Capture Text" to a file called Flight15.txt.
At the airfield, on my laptop,  I can see the telemetry coming into the terminal 
and know that it is all being recorded. Your telemetry file must
have a name ending in .txt.

To convert your telemetry into KML files you will need to Install Python. The code is 
currently tested on Windows using this download:-

http://www.python.org/ftp/python/2.6.2/python-2.6.2.msi

If you don't have python yet - I strongly recommend using that version.
If you already have another fairly recent version (2.5x,2.6.x ) stay with that.

Then you must download flan.py using subversion or a subversion client.
Make sure you have the directories called "models" and "images" below
the directory that contains flan.py. They are vital.

Double click on flan.py . You will be presented with a window where you
can choose your input telemetry file, and wayoints.h file. Add them in,
and when you have provided enough information, you will be able to press
the Start button.

The program will execute in about 2 seconds, and create one file.
For the example above, that would be flight15.kmz.
By the way, kmz files are simply zipped kml files, if you change the name from
flight15.kmz to flight15.zip, you can unzip the file and look at the kml markup yourself.

I do not recommend having more than 2000 telemetry line entries or Google Earth (GE)
may slow down and crawl.

If that is all set, then simply click on the kmz file (Flight15.kmz) in Windows to launch Google Earth.

Open the kmz file (in this example, flight15.kmz). 
When GE shows the file (Flight15.kmz) it will zoom into close to your flight area.

To see your whole flight, you must move the sliders in the "Time Animation Slider Controls" of
Google Earth towards the top of the screen. 

That is really it. From then on you are off and running. Have fun !

Pete Hollands, May 2010


