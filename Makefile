#PLAYER_INC = $(ROOTDIR)/$(LINUXDIR)/include 

DBGFLAG		= #-g
CFLAGS	:=  -I$(INCDIR) 
LDLIBS		= 
LOADLIBES	=
#RANLIB		= ranlib


#VPATH += ${SRC_DIR}/Core
#VPATH += ${SRC_DIR}/Devices/MediaServer

NEP_SOURCES = $(wildcard ${NEP_DIR}/Core/*.cpp)
NEP_SYS_SOURCES = $(wildcard ${NEP_DIR}/System/Posix/*.cpp)
NEP_SYS_SOURCES += $(wildcard ${NEP_DIR}/System/StdC/*.cpp)
NEP_SYS_SOURCES += $(wildcard ${NEP_DIR}/System/Bsd/*.cpp)
PLT_SOURCES = $(wildcard ${SRC_DIR}/Core/*.cpp)
PLT_MS_SOURCES = $(wildcard ${SRC_DIR}/Devices/MediaServer/*.cpp)
PLT_MR_SOURCES = $(wildcard ${SRC_DIR}/Devices/MediaRenderer/*.cpp)

player_source = $(wildcard ${ROOT_PATH}/mserver/*.cpp)
player_objs = $(player_source:.cpp=.o)
player_target = libmserver.a

id3tag_source = id3tag/APEV2Tag.c id3tag/ID3V1Tag.c id3tag/ID3V2Tag.c id3tag/common.c 
idd3tag_objs = $(id3tag_source:.c=.o)
id3tag_lib = libid3tag.a


TARGET = lib$(PLATINUM)

src = $(PLT_SOURCES) $(PLT_MS_SOURCES) $(PLT_MR_SOURCES) $(NEP_SOURCES) $(NEP_SYS_SOURCES)
objs = $(src:.cpp=.o)

OBJS = $(addprefix $(ROOT_PATH)/objs/, $(notdir $(objs)))

all: $(player_target) $(TARGET).a $(id3tag_lib) #move 
	$(CXX) $(CFLAGS) -L$(ROOT_PATH) mserver.cpp -lm -lid3tag -pthread -l$(PLATINUM) -lmserver -o audioserver

clean:
	rm -f $(ROOT_PATH)/objs/* $(TARGET).a
	find $(ROOT_PATH) -name *.o | xargs rm -rf

$(id3tag_objs): %.o : %.c
	$(CC) $(CFLAGS) -O2 -I$(INCDIR) -c $< -o $@ 

$(id3tag_lib) : $(id3tag_objs)
	$(RM) $@
	$(AR) r $@ $^
	$(RANLIB) $@
	
$(player_objs): %.o : %.cpp
	$(CXX) $(CFLAGS)  -O2 -I$(INCDIR) -c $< -o $@
	
$(player_target): $(player_objs)
	$(RM) $@
	$(AR) r $@ $^
	$(RANLIB) $@
	
move : $(src)
	for i in $(src); do\
		cp $$i ${ROOT_PATH}/objs/; \
	done
	
$(OBJS): %.o : %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET).a: $(OBJS)
	$(RM) $@
	$(AR) r $@ $^
	$(RANLIB) $@

