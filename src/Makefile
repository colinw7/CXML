CC = g++
AR = ar
RM = rm

CDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(LIB_DIR)/libCXML.a

SRC = \
CXMLComment.cpp \
CXML.cpp \
CXMLExecute.cpp \
CXMLNamedChar.cpp \
CXMLParser.cpp \
CXMLTag.cpp \
CXMLText.cpp \
CXMLToken.cpp \
CXMLVisitor.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
$(CDEBUG) \
-I$(INC_DIR) \
-I../../CRegExp/include \
-I../../CFile/include \
-I../../CStrUtil/include \
-I../../CUtil/include \
-I. \

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCXML.a

.SUFFIXES: .cpp

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(LIB_DIR)/libCXML.a: $(OBJS)
	$(AR) crv $(LIB_DIR)/libCXML.a $(OBJS)
