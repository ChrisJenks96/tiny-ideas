#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#
#https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html

#Ubuntu prereqs:
	#mesa-common-dev
	#libglfw3-dev

CC = gcc

CFLAGS = -Wall -g -s

INCLUDES =

LFLAGS =

LIBS =

SRCS = sb_server.c

OBJS = $(SRCS:.c=.o)

MAIN = spaceboostserver

.PHONY: depend clean

all:    $(MAIN)

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^