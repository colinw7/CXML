CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = .
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CXMLTest

SRC = \
CXMLTest.cpp

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-std=c++11 \
-I$(INC_DIR) \
-I../../CFile/include \
-I../../CUtil/include \
-I.

LFLAGS = \
-L$(LIB_DIR) \
-L../../CXML/lib \
-L../../CFile/lib \
-L../../COS/lib \
-L../../CRegExp/lib \
-L../../CStrUtil/lib \

LIBS = \
-lCXML -lCFile -lCOS -lCRegExp -lCStrUtil -ltre

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CXMLTest

.SUFFIXES: .cpp

.cpp.o:
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/CXMLTest: $(OBJS) $(LIB_DIR)/libCXML.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/CXMLTest $(OBJS) $(LFLAGS) $(LIBS)
