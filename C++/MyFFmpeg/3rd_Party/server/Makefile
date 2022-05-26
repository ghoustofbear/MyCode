#################################################
# Zhong caiwang <caiwang213@qq.com>
# Fri Feb 10 16:50:39 CST 2017
#
# makefile for building library media server
# 
#################################################

PWD        = $(shell pwd)/

CFGDIR     = $(PWD)config/
PRIVDIR    = $(PWD)private/
GWDIR      = $(PWD)gw/
PUDIR      = $(PWD)pu/
AUTHDIR    = $(PWD)auth/
EXPDIR     = $(TOPDIR)ui/server/include/

#lib target file
LIB        = libc2msv$(DSUFFIX).a

#cpp object files
CPPOBJS-y += $(PWD)server.o
CPPOBJS-y += $(PWD)session.o
CPPOBJS-y += $(PWD)sevent.o
CPPOBJS-y += $(PWD)bufferev.o
CPPOBJS-y += $(PWD)servmgr.o
CPPOBJS-y += $(PWD)system.o

CPPOBJS-y += $(PRIVDIR)privserver.o
CPPOBJS-y += $(PRIVDIR)servent.o
CPPOBJS-y += $(PRIVDIR)client.o
CPPOBJS-y += $(PRIVDIR)climgr.o
CPPOBJS-y += $(PRIVDIR)ssession.o
CPPOBJS-y += $(PRIVDIR)rsession.o

CPPOBJS-y += $(GWDIR)gsession.o
CPPOBJS-y += $(GWDIR)gsessmgr.o
CPPOBJS-y += $(GWDIR)supermgr.o
CPPOBJS-y += $(GWDIR)superior.o
CPPOBJS-y += $(GWDIR)hs_superior.o

CPPOBJS-y += $(GWDIR)platform.o
CPPOBJS-y += $(GWDIR)sipua.o
CPPOBJS-y += $(GWDIR)hs_sipua.o
CPPOBJS-y += $(GWDIR)sipcall.o
CPPOBJS-y += $(GWDIR)rtparser.o
CPPOBJS-y += $(GWDIR)rtpsession.o
CPPOBJS-y += $(GWDIR)rtspsession.o
CPPOBJS-y += $(GWDIR)rtspserver.o
CPPOBJS-y += $(GWDIR)hs_rtspsession.o

CPPOBJS-y += $(PUDIR)pu.o
CPPOBJS-y += $(PUDIR)gu.o
CPPOBJS-y += $(PUDIR)pumgr.o
CPPOBJS-y += $(PUDIR)putree.o
CPPOBJS-y += $(PUDIR)gumgr.o
CPPOBJS-y += $(PUDIR)hik_pu.o
CPPOBJS-y += $(PUDIR)hik_gu.o
CPPOBJS-y += $(PUDIR)hik_record_gu.o
CPPOBJS-y += $(PUDIR)gw_pu.o
CPPOBJS-y += $(PUDIR)gw_record_gu.o
CPPOBJS-y += $(PUDIR)hs_pu.o
CPPOBJS-y += $(PUDIR)msv_pu.o
CPPOBJS-y += $(PUDIR)msv_gu.o

CPPOBJS-y += $(AUTHDIR)user.o
CPPOBJS-y += $(AUTHDIR)usermgr.o
CPPOBJS-y += $(AUTHDIR)group.o
CPPOBJS-y += $(AUTHDIR)groupmgr.o

CPPOBJS-y += $(CFGDIR)observable.o
CPPOBJS-y += $(CFGDIR)cfg.o
CPPOBJS-y += $(CFGDIR)sqlitedb.o
CPPOBJS-y += $(CFGDIR)cfg_system.o
CPPOBJS-y += $(CFGDIR)cfg_platform.o
CPPOBJS-y += $(CFGDIR)cfg_user.o
CPPOBJS-y += $(CFGDIR)cfg_group.o
CPPOBJS-y += $(CFGDIR)cfg_user_group.o
CPPOBJS-y += $(CFGDIR)cfg_device.o
CPPOBJS-y += $(CFGDIR)view_user_group.o

#c object files
#COBJS-y   += $(COMDIR)uuid.o

SRCS      := $(CPPOBJS-y:.o=.cpp) $(COBJS-y:.o=.c)
OBJS      := $(addprefix ,$(CPPOBJS-y) $(COBJS-y))

CFLAGS    += -I$(PWD)
CFLAGS    += -I$(COMDIR)
CFLAGS    += -I$(CFGDIR)
CFLAGS    += -I$(PRIVDIR)
CFLAGS    += -I$(GWDIR)
CFLAGS    += -I$(PUDIR)
CFLAGS    += -I$(AUTHDIR)

CFLAGS    += -Wno-deprecated-declarations
CFLAGS    += -DDLL_EXPORT

$(LIB): .depend $(OBJS)
	$(AR) rv $@ $(OBJS)
	cp -f  $@ $(LIBDIR)
	cp -f  $(PWD)exports.h $(EXPDIR)
	cp -f  $(PWD)onuiupdate.h $(EXPDIR)
	cp -f  $(PWD)servmgr.h $(EXPDIR)
	cp -f  $(CFGDIR)cfg*.h $(EXPDIR)
	cp -f  $(CFGDIR)view*.h $(EXPDIR)
	cp -f  $(CFGDIR)observable.h $(EXPDIR)
	cp -f  $(CFGDIR)observer.h $(EXPDIR)
	cp -fr $(SRCDIR)/* $(SBINDIR)
ifeq ($(SHARE), yes)
	$(CXX) -shared -o $(LIBDIR)libc2msv$(DSUFFIX).so $(OBJS) -lc -lnsl -ldl -lresolv
	$(STRIP) $(LIBDIR)libc2msv$(DSUFFIX).so
endif

#########################################################################

# defines .depend target
include $(TOPDIR)rules.mk

-include .depend

#########################################################################
clean:
	rm -fr $(OBJS)
	rm -fr $(LIB)
	rm -fr libc2msv$(DSUFFIX).so
	rm -fr $(LIBDIR)$(LIB)
	rm -fr $(LIBDIR)libc2msv$(DSUFFIX).so
