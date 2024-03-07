CC       = gcc
CFLAGS   = -fPIC -O3
LFLAGS   =

SRCDIR   = src
OBJDIR   = build

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -Rf
	
all: build_buildDir $(OBJECTS)
	$(CC) -shared -Wl,-soname,librhd.so -o $(OBJDIR)/librhd.so $(OBJECTS) $(LFLAGS)
	ar rcs $(OBJDIR)/librhd.a $(OBJECTS) $(LFLAGS)

install: 
	cp $(OBJDIR)/librhd.so /usr/local/lib/.
	cp $(OBJDIR)/librhd.a /usr/local/lib/.
	cp src/rhd.h /usr/local/include/.

uninstall:
	rm -f /usr/lib/librhd.so
	rm -f /usr/lib/librhd.a
	rm -f /usr/include/rhd.h

test:
	cmake -Stests/ -Btests/build
	cmake --build tests/build && ctest --test-dir tests/build --output-on-failure
	
build_buildDir:
	@mkdir -p $(OBJDIR)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(rm) $(OBJDIR)	