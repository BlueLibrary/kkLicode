CC=gcc
CXX=g++

#����erizo��rtcת����������
CFLAGS = -I$(ERIZO_HOME)/erizo/src/erizo
CPPFLAGS = -std=c++11 
objects= RtcConmgr.o servermain.o
	
LDFLAGS   = -static -std=c++11 -fPIC -static-libgcc
ObjTARGET = servererizo

$(ObjTARGET):$(objects)
	$(CXX) $(LDFLAGS) -o $(ObjTARGET) $(objects) \
	$(LDFLAGS)
RtcConmgr.o:  RtcConmgr.h RtcConmgr.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS)  RtcConmgr.cpp
servermain: stdafx.h servermain.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS)  servermain.cpp
install:
	cp -f $(ObjTARGET) ./
clean:
	rm $(ObjTARGET) $(objects)