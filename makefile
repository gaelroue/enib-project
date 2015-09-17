
############################################################################
# project
############################################################################
EXEC=main

DESTDIR = /net/nfs-etu/vol/vFiler_NFS_ETU/Data/e2011/l1bouill/Document/S9/enib-project

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
  
%.o: %.c
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
