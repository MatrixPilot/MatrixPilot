uav_log_to_kml.py is designed to be a generic UAV Telemetry Analyzer.
It reads output from a telemetry file - and turns it into KML that
allows a flight to be analyzed using Google Earth (GE).

To see a picture of the possibilities - take a look at
http://picasaweb.google.co.uk/lh/photo/nfPkZ0y0-jWEBFdBulif_w?feat=directlink

As of early November 2009 this is early Alpha code. 

GETTING STARTED:
Using MatrixPilot  you must first configure
options.h to send the correct format of telemetry out of the serial port.

(If you have a genuine Ardustation telemetry file, you can also use that.
Progress is being made in supporting the Ardustation telemetry)

For MatrixPilot:-
In the options.h file, find the line that says :-
#define SERIAL_OUTPUT_FORMAT	SERIAL_NONE

and set it to :-
#define SERIAL_OUTPUT_FORMAT	SERIAL_UDB

Start up your plane and capture the telemetry into a file. (setting up 
wireless serial communications, or wired serial communications is beyond the scope
of this document).

For this example we will capture telemetry to flight15.TXT.
( In Windows I use Accessories / Communications / HyperTerminal,
In hyper terminal I do "Transfer / Capture Text" to a file called Flight15.Txt.
At the airfield, on my laptop,  I can see the telemetry coming into the terminal 
and know that it is all being recorded. At the moment, your telemetry file must
have a name ending in .TXT or .txt

To convert your telemetry into KML files you will need to Install Python. The code is 
currently tested on Windows using this download:-

http://www.python.org/ftp/python/2.6.2/python-2.6.2.msi

If you don't have python yet - I strongly recommend using that version.
If you already have another fairly recent version (2.5x,2.6.x ) stay with that.

Run up the Python Developer Graphical User Interface (GUI) called IDLE.
Open the python file. In the GUI for Python press F5 or select "Run / Module".

The program will execute in about 2 seconds, and create one file.
For the example above, that would be flight15.kmz.
By the way, kmz files are simply zipped kml files, if you change the name from
flight15.kmz to flight15.zip, you can unzip the file and look at the kml markup yourself.

I do not recommend having more than 2000 telemetry line entries or Google Earth (GE)
may slow down and crawl.

If that is all set, then simply click on the kmz file (Flight15.kmz) in Windows to launch Google Earth.

Open the kmz file (in this example, flight15.kmz). 
When GE shows the file (Flight15.kmz) it will zoom into close to your flight area.

Carefully look at the Places window on the left pane of GE. If you click on the main title
(Flight Log Flight15.TXT) GE will show you an overview of all the main parameters from options.h
for the UAV Devboard.

There are 3 folders below that main heading which are normally open on startup.
1) "The T3 Competition Course": Defaults to being off. Click on the green square if you would 
   like Red Pylons to mark the course for round 2 in October 2009

2) "Paths to Wayoints": Default to only showing the first 6 routes - otherwise the screen
   becomes very cluttered. You can turn the rest on by hand in places window. Just click
   on the green square to the left of "Paths to Wayoints". That will turn them all off.
   Click again, and then all your flight paths will be turned on. If you want to turn them
   on one by one, then click on the "+" to the left of "Paths to Waypoints", and the folder
   will open to let you control every "Path to Waypoint" individually.

3) Vectors: These default to be all on. A model place is plotted for each second of flight that
   reflects the full orientation and position of the plane from the point of view of the
   on board computer. Some additional information is displayed when you click on any individual
   vector in the places window.

Finally - you should know that you can edit all of the above by hand to add comments and analysis,
and then save the file  as a new kmz to send to a friend for ongoing discussion.
For example, if you click once on "Manual" in "Paths to Waypoints", and then right click on your mouse
to go to "Properties", you can add text, and also change the default snapshot view of that route
(very useful).

This code has been shared early in its life. The wisdom of Open Source Community bulding is
"Release Early, Release Often". 

Please send feedback / support issues to the UAV Devboard discussion group at:-


FEATURES: 
Easy to use - just click on the resulting KML and view your flight in Google Earth.

Color codes flight paths in Manual Mode as Grey. Color Codes each flight path towards 
the next waypoint - making it easy to determine when the plane is making decisions.

Provides Pylon waypoint markers relative to the origin of booting up the plane - for easy 
analysis of the T3 competition course.

Plots miniature planes in GE for each line of telemetry - showing the plane's heading,
roll, and pitch. These placemarks, when clicked in the places window of GE, open up
 to show additional information at the moment, such as GPS Course over the Ground,
 and GPS Speed over Ground.

REQUIREMENTS:
You really just need Python and Google Earth.

PLATFORMS. In theory the program will work on Mac OS and Linux but not tested. 
Although the resulting KML files are tested to work on Ubuntu Linux.

UAV TELEMETRY SUPPORT:
Works right now with MatrixPilot. The fancy features only work best 
with ...
#define SERIAL_OUTPUT_FORMAT	SERIAL_UDB
set in the options.h file. This is because the Ardustation telemetry (from Ardupilots) does not provide 
information  gyros, status modes, or time stamp information.

SUPPORT:
Please email your encouragement, questions,  constructive feedback
to the UAV Devboard mailing list at
http://groups.google.com/group/uavdevboard.

CURRENT TODOs:
1) Improve the initial GUI Window - make it more useable by non programmers.
2) Improve Ardustation support (need some telemetry files to write and test code).

Pete Hollands


