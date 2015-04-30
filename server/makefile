BINDIR=bin
OBJDIR=obj
SRCDIR=.

BINNAME=server
BIN=$(BINDIR)/$(BINNAME)
CLEAN=clean

DEBUGDIR=Debug
RELEASEDIR=Release

DEBUG=debug
RELEASE=release

GPP=g++
CFLAGS=-Wall -Wextra -Wmissing-braces -pedantic #-Werror
INCLUDE=-I"./"

CPPFILES=$(wildcard $(SRCDIR)/*.cpp)

OBJ=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CPPFILES))

OBJDEBUG=$(addprefix $(DEBUGDIR)/,$(OBJ))
OBJRELEASE=$(addprefix $(RELEASEDIR)/,$(OBJ))

BINDEBUG=$(addprefix $(DEBUGDIR)/,$(BIN))
BINRELEASE=$(addprefix $(RELEASEDIR)/,$(BIN))

FLAGSDEBUG=-g -pg
FLAGSRELEASE=-O2

LIBS=-lm -lssl -lcrypto -lpthread

$(DEBUG): $(OBJDEBUG)
	$(GPP) -o $(BINDEBUG) $(OBJDEBUG) $(LIBS)

$(RELEASE): $(OBJRELEASE)
	$(GPP) -o $(BINRELEASE) $(OBJRELEASE) $(LIBS)

$(CLEAN):
	rm -f $(BINDEBUG) $(BINRELEASE) $(OBJDEBUG) $(OBJRELEASE)

$(DEBUGDIR)/$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(GPP) $(CFLAGS) $(FLAGSDEBUG) $(INCLUDE) -c -o $@ $<

$(RELEASEDIR)/$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(GPP) $(CFLAGS) $(FLAGSRELEASE) $(INCLUDE) -c -o $@ $<

