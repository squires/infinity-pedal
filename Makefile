CCFLAGS = -Wall

all: mplayer

mplayer:
	${CC} ${CCFLAGS} -o pedal_mplayer mplayer.c event.c udev.c -ludev

clean::
	-rm -f *~ *.o pedal_mplayer

