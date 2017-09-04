CC = gcc
INCPATH = -I. -I../../Config -I../../libUDB -I../../libDCM -I../../MatrixPilot
RM_FILE = rm -f
ifeq ($(OS),Windows_NT)
	TARGET_EXTENSION = .exe
	FLAGS = -DWIN=1
	LFLAGS = 
	LIBS = -lws2_32
	SOCK_OBJECTS = ../HILSIM_XPlane/UDBSocketWin.o
	QT = "
else
	TARGET_EXTENSION = .out
	FLAGS = -DNIX=1
	LFLAGS =
	LIBS = -lm
	SOCK_OBJECTS = ../HILSIM_XPlane/UDBSocketUnix.o
	QT = '
endif
CFLAGS = -pipe -Wall -W -O0 $(FLAGS)

OBJECTS_DIR = ./

MPSIL_TARGET = matrixpilot$(TARGET_EXTENSION)
include udb.mk
include dcm.mk
include mp.mk
include sil.mk
MPSIL_OBJECTS = \
	$(UDB_OBJECTS) \
	$(DCM_OBJECTS) \
	$(MP_OBJECTS) \
	$(SIL_OBJECTS) \
	$(SOCK_OBJECTS)

MPCAT_TARGET = silcat$(TARGET_EXTENSION)
MPCAT_OBJECTS =  $(SOCK_OBJECTS) SILcat.o

first: all

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<
	$(CC) -MM $(CFLAGS) $(INCPATH) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e $(QT)s|.*:|$*.o:|$(QT) < $*.d.tmp > $*.d
	@sed -e $(QT)s/.*://$(QT) -e $(QT)s/\\$$//$(QT) < $*.d.tmp | \
		fmt -1 | sed -e $(QT)s/^ *//$(QT) -e $(QT)s/$$/:/$(QT) >> $*.d
	@rm -f $*.d.tmp

# pull in dependency info for *existing* .o files
-include $(MPSIL_OBJECTS:.o=.d)

all: $(MPSIL_TARGET) $(MPCAT_TARGET)

sil: $(MPSIL_TARGET)

cat: $(MPCAT_TARGET)

$(MPSIL_TARGET): $(MPSIL_OBJECTS)
	$(CC) -o $(MPSIL_TARGET) $(LFLAGS) $(MPSIL_OBJECTS) $(LIBS)

$(MPCAT_TARGET): $(MPCAT_OBJECTS)
	$(CC) -o $(MPCAT_TARGET) $(LFLAGS) $(MPCAT_OBJECTS) $(LIBS)

clean:
	-$(RM_FILE) $(MPSIL_OBJECTS) $(MPCAT_OBJECTS)
	-$(RM_FILE) *~ core *.core *.d ../../libDCM/*.d ../../MatrixPilot/*.d

