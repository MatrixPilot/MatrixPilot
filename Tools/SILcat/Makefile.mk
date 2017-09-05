CC = gcc
INCPATH = -I.
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
-include $(MPCAT_OBJECTS:.o=.d)

all: $(MPCAT_TARGET)

$(MPCAT_TARGET): $(MPCAT_OBJECTS)
	$(CC) -o $(MPCAT_TARGET) $(LFLAGS) $(MPCAT_OBJECTS) $(LIBS)

clean:
	-$(RM_FILE) $(MPCAT_OBJECTS)
	-$(RM_FILE) *~ core *.core *.d

