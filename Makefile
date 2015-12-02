POCO_VER := 1.6.1
POCO := lib/poco-$(POCO_VER)-all

MODULES := $(patsubst src/%,%,$(shell find src/* -maxdepth 0 -type d))
ALLSRCS := $(shell find src/ -type f -name "*.cpp")
MAIN := main
OBJDIR := build/obj
DEPDIR := build/dep
MODULESLIBS := $(patsubst %,lib%.so,$(MODULES))
INCLUDES := \
	-Iinclude \
	-I$(POCO)/Foundation/include \
	-I$(POCO)/Crypto/include \
	-I$(POCO)/Net/include \
	-I$(POCO)/NetSSL_OpenSSL/include \
	-I$(POCO)/Data/include \
	-I$(POCO)/Data/SQLite/include

CXX := clang
CXXFLAGS := -std=c++14 -g3 -fPIC $(INCLUDES) -fcolor-diagnostics
LINK := $(CXX)
LINKFLAGS := -g3 -lstdc++ -Wl,-rpath,. -Wl,-rpath,./$(POCO)/lib/Linux/x86_64 -fcolor-diagnostics

MAINOBJS := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(wildcard src/*.cpp))
LINKMODULES := -L. -lRemoteStorage
LINKPOCO := -L$(POCO)/lib/Linux/x86_64 -lPocoFoundationd -lPocoNetd -lPocoNetSSLd -lPocoDatad

$(MAIN): $(MAINOBJS) $(MODULESLIBS) $(POCO)
	@echo Creating $@...
	@$(LINK) $(LINKFLAGS) $(MAINOBJS) -o $@ $(LINKMODULES) $(LINKPOCO)

define MakeModuleRule
lib$(1).so: $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(wildcard src/$(1)/*.cpp))
	@echo Linking $$@...
	@$(LINK) --shared $(LINKFLAGS) $$^ -o $$@
endef

$(foreach module,$(MODULES),$(eval $(call MakeModuleRule,$(module))))

$(DEPDIR)/%.d:
	@mkdir -p $(dir $@)
	@g++ -nostdinc -std=c++14 -Iinclude -MF$@ -MG -MM -MP -MT$@ \
		-MT$(patsubst $(DEPDIR)/%.d,$(OBJDIR)/%.o,$@) $(patsubst $(DEPDIR)/%.d,src/%.cpp,$@)

-include $(patsubst src/%.cpp,$(DEPDIR)/%.d,$(ALLSRCS))

$(OBJDIR)/%.o: src/%.cpp $(POCO) Makefile
	@mkdir -p $(dir $@)
	@echo Compiling $<...
	@$(CXX) $(CXXFLAGS) $< -c -o $@

.PHONY: clean
clean:
	rm -rf build $(MAIN) $(MODULESLIBS)

.PHONY: distclean
distclean: clean
	rm -rf lib

# Poco:

POCO_OMIT := CppUnit,Data/MySQL,Data/ODBC,MongoDB,PageCompiler,Util,XML,

$(POCO): $(POCO).tar
	@mkdir -p lib
	@cd lib && tar -xf ../$^
	@cd $@ && ./configure --no-tests --no-samples --shared --omit=$(POCO_OMIT)
	@$(MAKE) -C $@ CC=$(CXX) CXX=$(CXX) LINKFLAGS="-lstdc++"

$(POCO).tar:
	@mkdir -p lib
	@cd lib && wget http://pocoproject.org/releases/poco-$(POCO_VER)/poco-$(POCO_VER)-all.tar.gz
	@gunzip $(POCO).tar.gz
