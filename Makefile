# Automatically Generated Makefile by EDE.
# For use with: make
#
# DO NOT MODIFY THIS FILE OR YOUR CHANGES MAY BE LOST.
# EDE is the Emacs Development Environment.
# http://cedet.sourceforge.net/ede.shtml
#

top="$(CURDIR)"/
ede_FILES=Project.ede Makefile

query_pcimap_wt_SOURCES=main.cc harixApp.cc database.cc home.cc lspci_query/pcimapQuery.cc lspci_query/pcimapResult.cc analyze_os/analyzeOS.cc pci_module/os_pci_module.cc pci_module/pci_moduleDB.cc
query_pcimap.wt_OBJ= main.o harixApp.o database.o home.o lspci_query/pcimapQuery.o lspci_query/pcimapResult.o analyze_os/analyzeOS.o pci_module/os_pci_module.o pci_module/pci_moduleDB.o
CXX= g++
CFLAGS= -g -Wall -Weffc++
LIBDIRS= -L/usr/local/lib
LIBS= -lwt \
-lwtfcgi \
-lmysqlcppconn
LDFLAGS= $(LIBDIRS) $(LIBS) -Wl,-rpath,/usr/local/lib
CXX_COMPILE=$(CXX) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
CXX_DEPENDENCIES=-Wp,-MD,.deps/$(*F).P
CXX_LINK=$(CXX) $(CFLAGS) $(LDFLAGS) -L.
VERSION=1.0
DISTDIR=$(top)Harix PreAlpha-$(VERSION)
top_builddir = 

DEP_FILES=.deps/main.P .deps/harixApp.P .deps/database.P .deps/home.P .deps/pcimapQuery.P .deps/pcimapResult.P .deps/analyzeOS.P .deps/os_pci_module.P .deps/pci_moduleDB.P .deps/pci_device.P .deps/harixApp.P .deps/home.P .deps/os_info.P .deps/pcimapQuery.P .deps/pcimapResult.P .deps/analyzeOS.P

all: query_pcimap.wt

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)
-include $(DEP_FILES)

%.o: %.cpp
	@echo '$(CXX_COMPILE) -c $<'; \
	$(CXX_COMPILE) $(CXX_DEPENDENCIES) -o $@ -c $<

query_pcimap.wt: $(query_pcimap.wt_OBJ)
	$(CXX_LINK) -o $@ $^ $(LDDEPS)

tags: 


clean:
	rm -f *.mod *.o *.obj .deps/*.P .lo

.PHONY: dist

dist:
	rm -rf $(DISTDIR)
	mkdir $(DISTDIR)
	cp $(query_pcimap_wt_SOURCES) $(ede_FILES) $(DISTDIR)
	tar -cvzf $(DISTDIR).tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)

Makefile: Project.ede
	@echo Makefile is out of date!  It needs to be regenerated by EDE.
	@echo If you have not modified Project.ede, you can use 'touch' to update the Makefile time stamp.
	@false



# End of Makefile
