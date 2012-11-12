
CXX=g++
CFLAGS=-g
INCLUDES=-I./list_aoi
SRC=list_aoi/list_aoi.cpp test.cpp
DEMO=demo

OBJ=$(subst .cpp,.o,$(SRC))  

%.o:%.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -o $@ -c $<

all:$(DEMO)
clean:
	rm $(OBJ) $(DEMO)

$(DEMO):$(OBJ)
	$(CXX) -o $@ $+
