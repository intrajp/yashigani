CC            = gcc
CFLAGS        = -O2 -Wall
DEST          = /usr/sbin
LDFLAGS       =
LIBS          =
OBJS          = yashigani.o
PROGRAM       = yashigani 

all:            $(PROGRAM)

$(PROGRAM):     $(OBJS)
		$(CC) -o $(PROGRAM) $(OBJS) $(LDFLAGS) $(LIBS)

clean:;         rm -f *.o *~ $(PROGRAM)

install:        $(PROGRAM)
		install -s $(PROGRAM) $(DEST)

