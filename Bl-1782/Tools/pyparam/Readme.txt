This readme includes information on generateDS and qgroundcontrol windows build.


Building qgroundcontrol in windows

Base isntructions are here:
http://www.qgroundcontrol.org/dev/build_source

There are additional requirements on the windows tools, especially if using MSVS2010 express.

Get MS visual C++ express 2010 here:
http://www.microsoft.com/visualstudio/en-us/products/2010-editions/express 

Install this version of QT
http://qt.nokia.com/downloads/windows-cpp-vs2010

You may also need to install Microsoft Windows SDK 7.0
As a web download: http://www.microsoft.com/download/en/details.aspx?displaylang=en&id=8279
or an iso: http://www.microsoft.com/download/en/details.aspx?displaylang=en&id=8442


generateDS

GenerateDS is used to build the parsers required for reading ParameterDatabase.xml
genParamDB.session is the generate DS file describing the build setup
GenParamDB.sh is a small linux script used to launch GenerateDS with genParamDB.session

Get generate ds here:
http://pypi.python.org/pypi/generateDS/

It has dependencies on lxml and xml2 libraries.