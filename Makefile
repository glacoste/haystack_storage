TARGET  = haystack
CC		= gcc
CFLAGS  = -Wall -std=c99  
LFLAGS  = -Wall

SRCDIR  = src
OBJDIR  = obj
BINDIR  = bin

SOURCES := $(wildcard $(SRCDIR)/*.c)
HEADERS := $(wildcard $(SRCDIR)/*.h)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(LFALGS) $(OBJECTS)

$(OBJECTS): $(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(BINDIR) $(OBJDIR)

clean:
	rm -rf data.blob data.blin input.txt $(BINDIR)/$(TARGET) $(OBJDIR)/*.o 
