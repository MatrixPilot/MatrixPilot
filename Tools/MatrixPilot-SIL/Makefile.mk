CC = gcc
INCPATH = -I. -I../../Config -I../../libUDB -I../../libDCM -I../../MatrixPilot -I../../FreeRTOS/include -I../../FreeRTOS/portable/MSVC-MingW

RM_FILE = rm -f
ifeq ($(OS),Windows_NT)
	TARGET_EXTENSION = .exe
	FLAGS = -DWIN=1
	LFLAGS = 
	LIBS = -lws2_32 -lWinmm
	SOCK_OBJECTS = ../HILSIM_XPlane/UDBSocketWin.o
	QT = "
else
	TARGET_EXTENSION = .out
	FLAGS = -DNIX=1
	LFLAGS = -Wl
	LIBS = -lm
	SOCK_OBJECTS = ../HILSIM_XPlane/UDBSocketUnix.o
	QT = '
endif
CFLAGS = -pipe -Wall -W -O0 $(FLAGS)

OBJECTS_DIR = ./

MPSIL_TARGET = matrixpilot$(TARGET_EXTENSION)
include udb.mk
include dcm.mk
include cntrl.mk
include mavlink.mk
include mp.mk
include sil.mk
MPSIL_OBJECTS = \
	$(UDB_OBJECTS) \
	$(DCM_OBJECTS) \
	$(CNTRL_OBJECTS) \
	$(MAVLINK_OBJECTS) \
	$(MP_OBJECTS) \
	$(SIL_OBJECTS) \
	$(SOCK_OBJECTS)

MPCAT_TARGET = silcat$(TARGET_EXTENSION)
MPCAT_OBJECTS =  $(SOCK_OBJECTS) SILcat.o

MPSILPLANE_TARGET = silplane$(TARGET_EXTENSION)
MPSILPLANE_OBJECTS =  $(SOCK_OBJECTS) SILplane.o

TEST_TARGET = test$(TARGET_EXTENSION)
TEST_OBJECTS =  $(SOCK_OBJECTS) TestSocket.o

first: all

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<
	$(CC) -MM $(CFLAGS) $(INCPATH) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e $(QT)s|.*:|$*.o:|$(QT) < $*.d.tmp > $*.d
	@sed -e $(QT)s/.*://$(QT) -e $(QT)s/\\$$//$(QT) < $*.d.tmp | \
		fmt -1 | sed -e $(QT)s/^ *//$(QT) -e $(QT)s/$$/:/$(QT) >> $*.d
	@rm -f $*.d.tmp

#%.o: %.c
#	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<
#	$(CC) -MM $(CFLAGS) $(INCPATH) $*.c > $*.d
#	@mv -f $*.d $*.d.tmp
#	@sed -e "s|.*:|$*.o:|" < $*.d.tmp > $*.d
#	@sed -e "s/.*://" -e "s/\\$$//" < $*.d.tmp | fmt -1 | sed -e "s/^ *//" -e "s/$$/:/" >> $*.d
#	@rm -f $*.d.tmp

#	@mv -f $*.d $*.d.tmp
#	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
#	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
#	@rm -f $*.d.tmp


# pull in dependency info for *existing* .o files
-include $(MPSIL_OBJECTS:.o=.d)

all: $(MPSIL_TARGET) $(MPCAT_TARGET) $(MPSILPLANE_TARGET)

sil: $(MPSIL_TARGET)

cat: $(MPCAT_TARGET)

test: $(TEST_TARGET)

silplane: $(MPSILPLANE_TARGET)

$(MPSIL_TARGET): $(MPSIL_OBJECTS)
	$(CC) -o $(MPSIL_TARGET) $(LFLAGS) $(MPSIL_OBJECTS) $(LIBS)

$(MPCAT_TARGET): $(MPCAT_OBJECTS)
	$(CC) -o $(MPCAT_TARGET) $(LFLAGS) $(MPCAT_OBJECTS) $(LIBS)

$(MPSILPLANE_TARGET): $(MPSILPLANE_OBJECTS)
	$(CC) -o $(MPSILPLANE_TARGET) $(LFLAGS) $(MPSILPLANE_OBJECTS) $(LIBS)

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) -o $(TEST_TARGET) $(LFLAGS) $(TEST_OBJECTS) $(LIBS)

clean:
	-$(RM_FILE) $(MPSIL_OBJECTS) $(MPCAT_OBJECTS)
	-$(RM_FILE) *~ core *.core *.d ../../libDCM/*.d ../../MatrixPilot/*.d

