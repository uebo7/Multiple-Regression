########################################################
# Author     : Logan Kaufman, Adrian Olweiler
########################################################

########################################################
# Variable definitions
########################################################
# C++ compiler
CXX := g++

# C++ compiler flags
# Use this first configuration for debugging
CXXFLAGS := -Wall -Werror -std=c++26 -ggdb -O0
# Use the following configuration for release
# CXXFLAGS := -O3 -Wall -Werror -std=c++26

# Linker: for C++ should be $(CXX)
LINK := $(CXX)

# Linker flags. Usually none.
LDFLAGS := 

# Library paths, prefaced with "-L". Usually none.
LDPATHS :=  

# Libraries we're using, prefaced with "-l".
LDLIBS := -ltbb -pthread

# Executable name. Needs to be the basename of your driver
#   file. I.e., your driver must be named $(EXEC).cc
EXEC := mult

#############################################################
# Rules
#   Rules have the form
#   target : prerequisites
#   	  recipe
#############################################################

# Add additional object files if you're using more than one
#   source file.
$(EXEC) : $(EXEC).o
	$(LINK) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Add rules for each object file.
# No recipes are typically needed.
search.o : mult.cpp 
#############################################################

.PHONY : clean
clean :
	@$(RM) $(EXEC) $(TEST) a.out core
	@$(RM) *.o *.d *~
	@echo "Cleanup complete."

#############################################################
