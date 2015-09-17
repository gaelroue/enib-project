
############################################################################
# project
############################################################################
EXEC=main

DESTDIR = /usr/bin
#
#	sub-directories and source files
#
SRC= main.c 

############################################################################
# C compiler & linker flags 
############################################################################
CFLAGS=-Wall -g 
LDFLAGS=

CC=gcc
DEPS = defs.h
############################################################################
# building rules
#
DEPDIR = .deps
OBJS= $(SRC:%.c=%.o)


all:
	mkdir -p $(DEPDIR)
	make $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
  
%.o: %.c ($DEPS)
	@$(CC) -MM $(CFLAGS) -o $(DEPDIR)/$*.d $<
	$(CC) -c $(CFLAGS) $<

-include $(SRC:%.c=$(DEPDIR)/%.d)

  
#
#	install
#
install:
	scp $(EXEC) root@192.168.0.2:$(DESTDIR)

#
#	uninstall
#
uninstall:

#
#	clean up everything
#
clean:
	rm *.o $(DEPDIR)/*.d $(EXEC)

############################################################################
