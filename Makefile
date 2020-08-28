#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = 
CFLAGS = -Wnon-virtual-dtor -Winit-self -Wcast-align -Wundef -Wfloat-equal -Wunreachable-code -Wmissing-include-dirs -Weffc++ -Wzero-as-null-pointer-constant -fstack-protector -fstack-clash-protection -Werror=format-security -Wl,-z,defs -Wl,-z,now -Wl,-z,relro
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = 

INC_DEBUG = $(INC) -Iinclude
CFLAGS_DEBUG = $(CFLAGS) -Wshadow -Wredundant-decls -Winline -Wswitch-enum -Wswitch-default -Wmain -Wall -std=c++17 -fPIC -Og -g -fexceptions -shared -fasynchronous-unwind-tables -fstack-protector-all -fcf-protection -D_FORTIFY_SOURCE=2 -D_GLIBCXX_ASSERTIONS
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/smallrdf.so

INC_RELEASE = $(INC) -Iinclude
CFLAGS_RELEASE = $(CFLAGS) -fomit-frame-pointer -std=c++17 -fPIC -fexceptions -shared -pipe -D_FORTIFY_SOURCE=1
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -O3 -s
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/smallrdf.so

INC_RELEASE_NATIVE = $(INC) -Iinclude
CFLAGS_RELEASE_NATIVE = $(CFLAGS) -fomit-frame-pointer -std=c++17 -fexceptions -shared -pipe -fpic -march=native -DNDEBUG -D_FORTIFY_SOURCE=1
RESINC_RELEASE_NATIVE = $(RESINC)
RCFLAGS_RELEASE_NATIVE = $(RCFLAGS)
LIBDIR_RELEASE_NATIVE = $(LIBDIR)
LIB_RELEASE_NATIVE = $(LIB)
LDFLAGS_RELEASE_NATIVE = $(LDFLAGS) -O3 -s
OBJDIR_RELEASE_NATIVE = obj/Release
DEP_RELEASE_NATIVE = 
OUT_RELEASE_NATIVE = bin/Release/smallrdf.so

INC_RELEASE_NATIVE_C = $(INC) -Iinclude
CFLAGS_RELEASE_NATIVE_C = $(CFLAGS) -fomit-frame-pointer -std=c17 -shared -pipe -fpic -march=native -DNDEBUG -D_FORTIFY_SOURCE=1
RESINC_RELEASE_NATIVE_C = $(RESINC)
RCFLAGS_RELEASE_NATIVE_C = $(RCFLAGS)
LIBDIR_RELEASE_NATIVE_C = $(LIBDIR)
LIB_RELEASE_NATIVE_C = $(LIB)
LDFLAGS_RELEASE_NATIVE_C = $(LDFLAGS) -O3 -s
OBJDIR_RELEASE_NATIVE_C = obj/Release
DEP_RELEASE_NATIVE_C = 
OUT_RELEASE_NATIVE_C = bin/Release/smallrdf.so

INC_TEST_DEBUG = $(INC) -Iinclude
CFLAGS_TEST_DEBUG = $(CFLAGS) -Wshadow -Wredundant-decls -Winline -Wswitch-enum -Wswitch-default -Wmain -Wall -std=c++17 -Og -g -fexceptions -fasynchronous-unwind-tables -fstack-protector-all -fpie -Wl,-pie -D_FORTIFY_SOURCE=2 -D_GLIBCXX_ASSERTIONS
RESINC_TEST_DEBUG = $(RESINC)
RCFLAGS_TEST_DEBUG = $(RCFLAGS)
LIBDIR_TEST_DEBUG = $(LIBDIR)
LIB_TEST_DEBUG = $(LIB)-lgtest -lpthread
LDFLAGS_TEST_DEBUG = $(LDFLAGS)
OBJDIR_TEST_DEBUG = obj/Debug
DEP_TEST_DEBUG = 
OUT_TEST_DEBUG = bin/Debug/test

OBJ_DEBUG = $(OBJDIR_DEBUG)/src/NTriplesParser.o $(OBJDIR_DEBUG)/src/NTriplesSerializer.o $(OBJDIR_DEBUG)/src/RDF.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/src/NTriplesParser.o $(OBJDIR_RELEASE)/src/NTriplesSerializer.o $(OBJDIR_RELEASE)/src/RDF.o

OBJ_RELEASE_NATIVE = $(OBJDIR_RELEASE_NATIVE)/src/NTriplesParser.o $(OBJDIR_RELEASE_NATIVE)/src/NTriplesSerializer.o $(OBJDIR_RELEASE_NATIVE)/src/RDF.o

OBJ_RELEASE_NATIVE_C = $(OBJDIR_RELEASE_NATIVE_C)/src/NTriplesParser.o $(OBJDIR_RELEASE_NATIVE_C)/src/NTriplesSerializer.o $(OBJDIR_RELEASE_NATIVE_C)/src/RDF.o

OBJ_TEST_DEBUG = $(OBJDIR_TEST_DEBUG)/src/NTriplesParser.o $(OBJDIR_TEST_DEBUG)/src/NTriplesSerializer.o $(OBJDIR_TEST_DEBUG)/src/RDF.o $(OBJDIR_TEST_DEBUG)/test/NTriplesParser_test.o $(OBJDIR_TEST_DEBUG)/test/NTriplesSerializer_test.o $(OBJDIR_TEST_DEBUG)/test/RDF_test.o $(OBJDIR_TEST_DEBUG)/test/test.o

all: debug release release_native release_native_c test_debug

clean: clean_debug clean_release clean_release_native clean_release_native_c clean_test_debug

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) -shared $(LIBDIR_DEBUG) $(OBJ_DEBUG)  -o $(OUT_DEBUG) $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/src/NTriplesParser.o: src/NTriplesParser.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/NTriplesParser.cpp -o $(OBJDIR_DEBUG)/src/NTriplesParser.o

$(OBJDIR_DEBUG)/src/NTriplesSerializer.o: src/NTriplesSerializer.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/NTriplesSerializer.cpp -o $(OBJDIR_DEBUG)/src/NTriplesSerializer.o

$(OBJDIR_DEBUG)/src/RDF.o: src/RDF.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/RDF.cpp -o $(OBJDIR_DEBUG)/src/RDF.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)/src

before_release: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) -shared $(LIBDIR_RELEASE) $(OBJ_RELEASE)  -o $(OUT_RELEASE) $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/src/NTriplesParser.o: src/NTriplesParser.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/NTriplesParser.cpp -o $(OBJDIR_RELEASE)/src/NTriplesParser.o

$(OBJDIR_RELEASE)/src/NTriplesSerializer.o: src/NTriplesSerializer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/NTriplesSerializer.cpp -o $(OBJDIR_RELEASE)/src/NTriplesSerializer.o

$(OBJDIR_RELEASE)/src/RDF.o: src/RDF.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/RDF.cpp -o $(OBJDIR_RELEASE)/src/RDF.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE)/src

before_release_native: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE_NATIVE)/src || mkdir -p $(OBJDIR_RELEASE_NATIVE)/src

after_release_native: 

release_native: before_release_native out_release_native after_release_native

out_release_native: before_release_native $(OBJ_RELEASE_NATIVE) $(DEP_RELEASE_NATIVE)
	$(LD) -shared $(LIBDIR_RELEASE_NATIVE) $(OBJ_RELEASE_NATIVE)  -o $(OUT_RELEASE_NATIVE) $(LDFLAGS_RELEASE_NATIVE) $(LIB_RELEASE_NATIVE)

$(OBJDIR_RELEASE_NATIVE)/src/NTriplesParser.o: src/NTriplesParser.cpp
	$(CXX) $(CFLAGS_RELEASE_NATIVE) $(INC_RELEASE_NATIVE) -c src/NTriplesParser.cpp -o $(OBJDIR_RELEASE_NATIVE)/src/NTriplesParser.o

$(OBJDIR_RELEASE_NATIVE)/src/NTriplesSerializer.o: src/NTriplesSerializer.cpp
	$(CXX) $(CFLAGS_RELEASE_NATIVE) $(INC_RELEASE_NATIVE) -c src/NTriplesSerializer.cpp -o $(OBJDIR_RELEASE_NATIVE)/src/NTriplesSerializer.o

$(OBJDIR_RELEASE_NATIVE)/src/RDF.o: src/RDF.cpp
	$(CXX) $(CFLAGS_RELEASE_NATIVE) $(INC_RELEASE_NATIVE) -c src/RDF.cpp -o $(OBJDIR_RELEASE_NATIVE)/src/RDF.o

clean_release_native: 
	rm -f $(OBJ_RELEASE_NATIVE) $(OUT_RELEASE_NATIVE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE_NATIVE)/src

before_release_native_c: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE_NATIVE_C)/src || mkdir -p $(OBJDIR_RELEASE_NATIVE_C)/src

after_release_native_c: 

release_native_c: before_release_native_c out_release_native_c after_release_native_c

out_release_native_c: before_release_native_c $(OBJ_RELEASE_NATIVE_C) $(DEP_RELEASE_NATIVE_C)
	$(LD) -shared $(LIBDIR_RELEASE_NATIVE_C) $(OBJ_RELEASE_NATIVE_C)  -o $(OUT_RELEASE_NATIVE_C) $(LDFLAGS_RELEASE_NATIVE_C) $(LIB_RELEASE_NATIVE_C)

$(OBJDIR_RELEASE_NATIVE_C)/src/NTriplesParser.o: src/NTriplesParser.cpp
	$(CXX) $(CFLAGS_RELEASE_NATIVE_C) $(INC_RELEASE_NATIVE_C) -c src/NTriplesParser.cpp -o $(OBJDIR_RELEASE_NATIVE_C)/src/NTriplesParser.o

$(OBJDIR_RELEASE_NATIVE_C)/src/NTriplesSerializer.o: src/NTriplesSerializer.cpp
	$(CXX) $(CFLAGS_RELEASE_NATIVE_C) $(INC_RELEASE_NATIVE_C) -c src/NTriplesSerializer.cpp -o $(OBJDIR_RELEASE_NATIVE_C)/src/NTriplesSerializer.o

$(OBJDIR_RELEASE_NATIVE_C)/src/RDF.o: src/RDF.c
	$(CC) $(CFLAGS_RELEASE_NATIVE_C) $(INC_RELEASE_NATIVE_C) -c src/RDF.c -o $(OBJDIR_RELEASE_NATIVE_C)/src/RDF.o

clean_release_native_c: 
	rm -f $(OBJ_RELEASE_NATIVE_C) $(OUT_RELEASE_NATIVE_C)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE_NATIVE_C)/src

before_test_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_TEST_DEBUG)/src || mkdir -p $(OBJDIR_TEST_DEBUG)/src
	test -d $(OBJDIR_TEST_DEBUG)/test || mkdir -p $(OBJDIR_TEST_DEBUG)/test

after_test_debug: 

test_debug: before_test_debug out_test_debug after_test_debug

out_test_debug: before_test_debug $(OBJ_TEST_DEBUG) $(DEP_TEST_DEBUG)
	$(LD) $(LIBDIR_TEST_DEBUG) -o $(OUT_TEST_DEBUG) $(OBJ_TEST_DEBUG)  $(LDFLAGS_TEST_DEBUG) $(LIB_TEST_DEBUG)

$(OBJDIR_TEST_DEBUG)/src/NTriplesParser.o: src/NTriplesParser.cpp
	$(CXX) $(CFLAGS_TEST_DEBUG) $(INC_TEST_DEBUG) -c src/NTriplesParser.cpp -o $(OBJDIR_TEST_DEBUG)/src/NTriplesParser.o

$(OBJDIR_TEST_DEBUG)/src/NTriplesSerializer.o: src/NTriplesSerializer.cpp
	$(CXX) $(CFLAGS_TEST_DEBUG) $(INC_TEST_DEBUG) -c src/NTriplesSerializer.cpp -o $(OBJDIR_TEST_DEBUG)/src/NTriplesSerializer.o

$(OBJDIR_TEST_DEBUG)/src/RDF.o: src/RDF.cpp
	$(CXX) $(CFLAGS_TEST_DEBUG) $(INC_TEST_DEBUG) -c src/RDF.cpp -o $(OBJDIR_TEST_DEBUG)/src/RDF.o

$(OBJDIR_TEST_DEBUG)/test/NTriplesParser_test.o: test/NTriplesParser_test.cpp
	$(CXX) $(CFLAGS_TEST_DEBUG) $(INC_TEST_DEBUG) -c test/NTriplesParser_test.cpp -o $(OBJDIR_TEST_DEBUG)/test/NTriplesParser_test.o

$(OBJDIR_TEST_DEBUG)/test/NTriplesSerializer_test.o: test/NTriplesSerializer_test.cpp
	$(CXX) $(CFLAGS_TEST_DEBUG) $(INC_TEST_DEBUG) -c test/NTriplesSerializer_test.cpp -o $(OBJDIR_TEST_DEBUG)/test/NTriplesSerializer_test.o

$(OBJDIR_TEST_DEBUG)/test/RDF_test.o: test/RDF_test.cpp
	$(CXX) $(CFLAGS_TEST_DEBUG) $(INC_TEST_DEBUG) -c test/RDF_test.cpp -o $(OBJDIR_TEST_DEBUG)/test/RDF_test.o

$(OBJDIR_TEST_DEBUG)/test/test.o: test/test.cpp
	$(CXX) $(CFLAGS_TEST_DEBUG) $(INC_TEST_DEBUG) -c test/test.cpp -o $(OBJDIR_TEST_DEBUG)/test/test.o

clean_test_debug: 
	rm -f $(OBJ_TEST_DEBUG) $(OUT_TEST_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_TEST_DEBUG)/src
	rm -rf $(OBJDIR_TEST_DEBUG)/test

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release before_release_native after_release_native clean_release_native before_release_native_c after_release_native_c clean_release_native_c before_test_debug after_test_debug clean_test_debug

