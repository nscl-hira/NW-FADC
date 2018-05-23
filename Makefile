# Makefile for FADC analysis
# Adam Anthony
# May 5, 2018
# Generates a shared object of name $(LIBNAME), including the
# correct ROOT dictionary. Every source (.cc or.cpp) file in $(SRCDIR) is compiled
# into the shared object. The outputed object is in $(LIBDIR).
#
# Requires the LinkDef.h file to have the form $(LIBNAME)LinkDef.h and
# be in the base directory.
#
# Files of type .cpp will be build into the shared objects
# Files of type .cc are source code that can be build into an executable

###############################
##### User specified crap #####
###############################

# Specify the the binary, build, and source directories
BUILDDIR = build
BINDIR   = bin
SRCDIR   = src
LIBDIR   = lib
INCLDIR  = include

#Set the name of the shared library and dictionary
LIBNAME = DataStruct

#################################
##### System Specified crap #####
#################################

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
OBJS = $(patsubst $(SRCDIR)%, $(BUILDDIR)%, $(filter %.o, $(SRCS:.cpp=.o )))

SO = $(LIBDIR)/lib$(LIBNAME).so
DICTO = $(BUILDDIR)/$(LIBNAME)Dict.o

#Set compiler flags
CXXFLAGS = $(shell $(RC) --cflags) -fPIC -std=c++11 -I$(INCLDIR) -g
SOFLAGS = --shared -std=c++11 -g
LDFLAGS = $(shell $(RC) --ldflags --libs) -L$(LIBDIR) -g
EXEFLAGS = $(shell $(RC) --cflags --libs) -I$(INCLDIR) -g

#***************#
#**** RULES ****#
#***************#

default: all

all: $(SO) $(BINDIR)/FADC_Conversion_root $(BINDIR)/TCB_Conversion_root
sharedObject: $(SO)

#Compile and link executables
$(BINDIR)/FADC_Conversion_root: $(SRCDIR)/FADC_Conversion_root_v1.cc $(OBJS) $(DICTO)
	@echo "Building $@..."
	@$(CXX) $(EXEFLAGS) -o $@ $^
$(BINDIR)/TCB_Conversion_root: $(SRCDIR)/TCB_Conversion_root_v1.cc $(OBJS) $(DICTO)
	@echo "Building $@..."
	@$(CXX) $(EXEFLAGS) -o $@ $^

#Compile and link shared object
$(SO): $(OBJS) $(DICTO)
	@echo "Assembling the shared object $(LIBNAME)..."
	@$(CXX) $(SOFLAGS) $(LDFLAGS) -o $@ $^


#Rules to build all object files
$(BUILDDIR)/%.o : $(SRCDIR)/%.cc $(INCLS)
	@echo "Building $@..."
	@$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILDDIR)/%.o : $(SRCDIR)/%.cpp $(INCLS)
	@echo "Building $@..."
	@$(CXX) $(CXXFLAGS) -c -o $@ $<


#Rules for dictionary generation
$(DICTO): $(INCLS)
	@echo "Generating root library..."
	@rootcint -f $(LIBNAME)Dict.cxx -c $^ $(LIBNAME)LinkDef.h
	@mv $(LIBNAME)Dict_rdict.pcm $(LIBDIR)/
	@$(CXX) $(CXXFLAGS) -c -o $@ $(LIBNAME)Dict.cxx
	@rm -f $(LIBNAME)Dict.cxx


.PHONY: clean
clean:
	@rm -rf *.so *.o *.pcm *Dict.cxx
	@rm -rf $(BUILDDIR)/*
	@rm -rf $(LIBDIR)/*	
	@rm -rf $(BINDIR)/*

.PHONY: test
test:
	@echo $(INCLS)
	@echo $(SRCS)
	@echo $(OBJS)
