#****************************************************************************
# 通用的生成可执行文件的 c/c++ GNU Makefile 
# Writen By ZhangBaoZhu,2007.7
# Alter By  elva,2010.3
# 功能：自动从所有的.c或.cpp文件列表中生成输出文件，生成可执行
#注意： 所有路径变量须以 /结尾
#****************************************************************************

#常用定义选项
PROGRAM_NAME		= hls_dl
SRC_PATH		= ../src/
SRC_FILES		=  main.cpp cdownloader.cpp creadline.cpp cm3u8parse.cpp csavefile.cpp  HttpClient.cpp \
                                zevent.cpp DlBuffer.cpp utils.cpp cudpsend.cpp TsPacket.cpp cudpobj.cpp SafeDeque.cpp

OUTPUT_PATH		= 

USING_LIBS		= -lcurl -lpthread
USING_INCLUDES_PATH	= -I/home/ovtsvn/linux_devlibs/include
USING_LIBS_PATH		= -L/home/ovtsvn/linux_devlibs/bin/x86_64/debug

DEBUG  			= YES
DEFINES			= 



#通用定义
GCC			= g++
LD 			= g++

ZMOD_SVNVER                     = $(shell svnversion . | sed "s/.*://")
ZMOD_BUILD_DATE         = $(shell date)

DEBUG_CFLAGS     := -c -Wall  -Wno-format -g -DDEBUG
RELEASE_CFLAGS   := -c -Wall   -Wno-unknown-pragmas -Wno-format -O3



DEBUG_CFLAGS    += -DZMOD_SVNVER="\"$(ZMOD_SVNVER)\"" -DZMOD_BUILD_DATE="\"$(ZMOD_BUILD_DATE)\"" -DZMOD_VERSION="\"$(ZMOD_VERSION)\""
RELEASE_CFLAGS  += -DZMOD_SVNVER="\"$(ZMOD_SVNVER)\"" -DZMOD_BUILD_DATE="\"$(ZMOD_BUILD_DATE)\"" -DZMOD_VERSION="\"$(ZMOD_VERSION)\""


DEBUG_LDFLAGS    := -g  -fPIC -Wl,-soname,$(PROGRAM_NAME)
RELEASE_LDFLAGS  := -fPIC -Wl,-soname,$(PROGRAM_NAME)


ifeq (YES, ${DEBUG})
	CFLAGS       	:= ${DEBUG_CFLAGS}
	LDFLAGS      	:= ${DEBUG_LDFLAGS}
	USING_LIBS_PATH	:= $(USING_LIBS_PATH)
else
	CFLAGS      	:= ${RELEASE_CFLAGS}
	LDFLAGS      	:= ${RELEASE_LDFLAGS}
	USING_LIBS_PATH	:= $(addsuffix release/,$(USING_LIBS_PATH))
endif

#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS		+= $(DEFINES)
CFLAGS		+= $(USING_INCLUDES_PATH)

#****************************************************************************
# 定义自动化编译宏，使用GCC方式
#****************************************************************************
# $(call make-depend,source-file,object-file,depend-file)
define make-depend
	$(GCC) -MM -MF $3 -MP -MT $2 $(CFLAGS) $1
endef

OBJS 		= $(addsuffix .o,$(basename $(SRC_FILES)))
DEPENDS 	= $(addsuffix .d,$(basename $(SRC_FILES)))
OUTPUT 		= $(OUTPUT_PATH)$(PROGRAM_NAME)

VPATH		= $(SRC_PATH)

default:$(OUTPUT)
	@echo "$(OUTPUT) MAKE 处理完毕"

$(OUTPUT):$(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(USING_LIBS_PATH) $(USING_LIBS)
	@echo "\\33[35m\\33[1m[链接]\\33[21m $@ 执行完毕，总计更新处理：$(words $?) 个文件\\33[0m"

%.o:%.c
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(GCC) $(CFLAGS) $< -o $@
	@echo "\\33[32m\\33[1m[编译]\\33[21m $< 执行完毕\\33[0m"
	
%.o:%.cpp
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@$(GCC) $(CFLAGS) $< -o $@
	@echo "\\33[32m\\33[1m[编译]\\33[21m $< 执行完毕\\33[0m"

-include $(DEPENDS)

help:
	@echo "\\33[1m\\33[33m============== 通用可执行文件MAKEFILE ==============\\33[0m"
	@echo "\\33[33m  2007.2 Writen By ZhuFeng \\33[0m"
	@echo "\\33[32m\\33[1mmake		:\\33[21m 缺省调用方式，使用debug模式进行编译与连接，输出二进制文件到指定目录\\33[0m"
	@echo "\\33[32m\\33[1mmake DEBUG=NO	:\\33[21m 使用release模式进行编译与连接，输出二进制文件到指定目录 \\33[0m"
	@echo "\\33[32m\\33[1mmake help	:\\33[21m显示帮助信息\\33[0m"
	@echo "\\33[32m\\33[1mmake rebuild	:\\33[21m重新生成所有文件\\33[0m"
	@echo "\\33[32m\\33[1mmake clean	:\\33[21m清除生成的中间文件\\33[0m"


rebuild:clean default
	@echo "\\33[34m\\33[1m[rebuild]\\33[21m 执行完毕\\33[0m"
clean:
	@rm -f *.o
	@rm -f *.d
	@rm -f $(OUTPUT)
	@echo "\\33[34m\\33[1m[clean]\\33[21m 执行完毕\\33[0m"
	
