# Makefile for FADC analysis
# Adam Anthony
# May 5, 2018
# Generates a shared object of name $(LIBNAME), including the
# correct ROOT dictionary. Every source (.cc or.cpp) file in $(SRCDIR) is compiled
# into the shared object. The outputed object is in $(LIBDIR).
#
# Requires the LinkDef.h file to have the form $(LIBNAME)LinkDef.h and
# be in the base directory.



# Specify the the binary, build, and source directories
BUILDDIR = build
BINDIR   = bin
SRCDIR   = src
LIBDIR   = lib
INCLDIR  = include

#Set the name of the shared library and dictionary
LIBNAME = DataStruct

#Grab info on ROOT and compiler
RC := root-config
ROOTSYS := $(shell $(RC) --prefix)
CXX := $(shell $(RC) --cxx)

#Grab all of the headers
INCLS  = $(wildcard $(INCLDIR)/*.h)

#Grab all of the source files to be compiled into lib
SRCS  = $(wildcard $(SRCDIR)/*.cpp)
SRCS += $(wildcard $(SRCDIR)/*.cc)


#Turn create list of expected OBJ files
OBJS = $(patsubst $(SRCDIR)%, $(BUILDDIR)%, $(filter %.o, $(SRCS:.cpp=.o ) $(SRCS:.cc=.o )))

SO = $(LIBDIR)/lib$(LIBNAME).so
DICTO = $(BUILDDIR)/$(LIBNAME)Dict.o

#Set compiler flags
CXXFLAGS = $(shell $(RC) --cflags) -fPIC -std=c++11 -I$(INCLDIR)
SOFLAGS = --shared -std=c++11 
LDFLAGS = $(shell $(RC) --ldflags --libs) -L$(LIBDIR)
EXEFLAGS = $(shell $(RC) --cflags --libs) -I$(INCLDIR)

#***************#
#**** RULES ****#
#***************#

default: all

all: $(SO) FADC_Conversion_root TCB_Conversion_root

#Compile and link executables
FADC_Conversion_root: FADC_Conversion_root_v1.cc $(OBJS) $(DICTO)
	$(CXX) $(EXEFLAGS) -o $(BINDIR)/$@ $^
TCB_Conversion_root: TCB_Conversion_root_v1.cc $(OBJS) $(DICTO)
	$(CXX) $(EXEFLAGS) -o $(BINDIR)/$@ $^

#Compile and link shared object
$(SO): $(OBJS) $(DICTO)
	$(CXX) $(SOFLAGS) $(LDFLAGS) -o $@ $^


#Rules to build all object files
$(BUILDDIR)/%.o : $(SRCDIR)/%.cc $(INCLS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILDDIR)/%.o : $(SRCDIR)/%.cpp $(INCLS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o : %.cc $(INCLS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
%.o : %.cpp $(INCLS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

#Rules for dictionary generation
$(DICTO): $(INCLS) 
	rootcint -f $(LIBNAME)Dict.cxx -c $^ $(LIBNAME)LinkDef.h
	mv $(LIBNAME)Dict_rdict.pcm $(LIBDIR)/
#	g++ -o $@ -fPIC -c Dict.cxx `root-config --cflags`
	$(CXX) $(CXXFLAGS) -c -o $@ $(LIBNAME)Dict.cxx
	rm -f $(LIBNAME)Dict.cxx


.PHONY: clean
clean:
	rm -rf *.so *.o *.pcm *Dict.cxx FADC_Conversion_root TCB_Conversion_root
	rm -rf $(BUILDDIR)/*
	rm -rf $(LIBDIR)/*
	rm -rf $(BINDIR)/*

.PHONY: test
test:
	@echo $(INCLS)
	@echo $(SRCS)
	@echo $(OBJS)
