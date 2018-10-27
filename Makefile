CC            = gcc
CFLAGS        = -O2 -Wall
DEST          = /usr/sbin
LDFLAGS       =
LIBS          =
OBJS          = fanotify.o
PROGRAM       = fanotify 

all:            $(PROGRAM)

$(PROGRAM):     $(OBJS)
		$(CC) -o $(PROGRAM) $(OBJS) $(LDFLAGS) $(LIBS)

clean:;         rm -f *.o *~ $(PROGRAM)

install:        $(PROGRAM)
		install -s $(PROGRAM) $(DEST)

