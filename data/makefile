CC = gcc
CFLAGS += -g -std=c99 -Wall
LDLIBS = -pthread 
INCLUDES = -I ./headers


OBJDIR = obj
SRCDIR = src
BINDIR = bin
HEADDIR = headers

SRC = queue.c main.c utils.c file.c collector.c workers.c masterworker.c generafile.c

OBJS = $(OBJDIR)/queue.o $(OBJDIR)/main.o $(OBJDIR)/utils.o $(OBJDIR)/file.o $(OBJDIR)/collector.o $(OBJDIR)/workers.o $(OBJDIR)/masterworker.o



.PHONY: clean test

TARGETS = farm generafile 

	
all: $(TARGETS)



clean:
	@rm $(OBJS)
	@rm -f $(BINDIR)/file*
	@rm -f $(BINDIR)/*.txt
	@rm -f -r $(BINDIR)/testdir

test: $(BINDIR)/farm $(BINDIR)/generafile $(BINDIR)/test.sh
	cd $(BINDIR);chmod +x test.sh;./test.sh 
	






$(OBJDIR)/queue.o: $(SRCDIR)/queue.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ 

$(OBJDIR)/main.o: $(SRCDIR)/main.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/file.o: $(SRCDIR)/file.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/collector.o: $(SRCDIR)/collector.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/workerpool.o: $(SRCDIR)/workerpool.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/utils.o: $(SRCDIR)/utils.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


$(OBJDIR)/workers.o: $(SRCDIR)/workers.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/masterworker.o: $(SRCDIR)/masterworker.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@




$(SRCDIR)/%.c: 
	@mkdir -p $(SRCDIR)
	echo "sposto i file .c nella cartella src"
	@-mv -f $(SRC) $(SRCDIR)





farm: $(OBJS) 
	@mkdir -p $(BINDIR)
	@-mv ./test.sh $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/farm  $(OBJS) $(LDLIBS)

generafile: 
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(SRCDIR)/generafile.c -o $(BINDIR)/generafile


