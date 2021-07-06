	
#CC=$(CROSS_COMPILE)gcc
#AR=$(CROSS_COMPILE)ar

TAR_FILE = GStremerTest

SRC_FILES=$(shell find ./ -name "*.c" -print)
OBJ_FILES = $(patsubst %.c,%.o,$(SRC_FILES))

#SRC_FILES= PlayH264.c
#OBJ_FILES = PlayH264.o

LIBXML_INC=$(shell pkg-config --cflags libgstreamer-1.0)
CFLAGS += $(LIBXML_INC)

LIBXML_LIBS=$(shell pkg-config --libs libgstreamer-1.0)
LIBS += $(LIBXML_LIBS)

#LIB_PATH =./
#LIB_FILES = -lhantro
#LIB_FILES = -lg1

all: $(TAR_FILE)

install: install_headers
	#@mkdir -p $(DEST_DIR)/usr/lib
	#cp -P $(LIBNAME).* $(DEST_DIR)/usr/lib

install_headers:
	#@mkdir -p $(DEST_DIR)/usr/include
	#cp vpu_lib.h $(DEST_DIR)/usr/include
	#cp vpu_io.h $(DEST_DIR)/usr/include

%.o: %.c
	$(CC)  -O2 -c $^ -o $@

$(TAR_FILE): $(OBJ_FILES)
	$(CC)  $^ -o $@ $(CFLAGS) $(LIBS) $<
	#$(CC)  $^ -o $@ -lpthread -L$(LIB_PATH) $(LIB_FILES)

.PHONY: clean
clean:
	rm -f $(TAR_FILE).* $(OBJ_FILES)