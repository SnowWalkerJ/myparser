TARGET_DIR=build
SOURCE_DIR=myparser
CXX=g++
LEX=lex
YACC=yacc
CXXFLAGS=-std=c++11
HEADERS=$(SOURCE_DIR)/interpreter.hpp $(SOURCE_DIR)/common.hpp $(SOURCE_DIR)/y.tab.h
LEX_SOURCE=$(SOURCE_DIR)/lex.l
YACC_SOURCE=$(SOURCE_DIR)/yacc.y
LEX_TARGET=$(SOURCE_DIR)/lex.yy.cc
YACC_TARGET=$(SOURCE_DIR)/y.tab.c

EXE=$(TARGET_DIR)/run

CXX_FILES=$(SOURCE_DIR)/interpreter.cpp
O_FILES=$(TARGET_DIR)/interpreter.o $(TARGET_DIR)/lex.yy.o \
$(TARGET_DIR)/y.tab.o

.Phony: all run clean

$(LEX_TARGET): $(LEX_SOURCE) $(SOURCE_DIR)/common.hpp $(SOURCE_DIR)/y.tab.h
	$(LEX) -o $@ $<

$(YACC_TARGET): $(YACC_SOURCE) $(SOURCE_DIR)/common.hpp
	$(YACC) -d -o $@ $<

$(SOURCE_DIR)/y.tab.h: $(YACC_SOURCE) $(SOURCE_DIR)/common.hpp
	$(YACC) -d -o $@ $<

$(TARGET_DIR)/interpreter.o: $(SOURCE_DIR)/interpreter.cpp $(SOURCE_DIR)/interpreter.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET_DIR)/lex.yy.o: $(SOURCE_DIR)/lex.yy.cc $(SOURCE_DIR)/common.hpp $(SOURCE_DIR)/y.tab.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET_DIR)/y.tab.o: $(SOURCE_DIR)/y.tab.c $(SOURCE_DIR)/common.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXE): $(O_FILES)
	$(CXX) $(CXXFLAGS) $^ -o $@


all: run

run: $(EXE)


clean:
	rm build/*
	rm $(SOURCE_DIR)/lex.yy.cc $(SOURCE_DIR)/y.tab.c $(SOURCE_DIR)/y.tab.h
