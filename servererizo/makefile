CC=gcc
CXX=g++

#基于erizo的rtc转发服务器。
CFLAGS = -I$(BUILD_DIR)/libdeps/build/include -I$(ERIZO_HOME)/src/third_party/webrtc/src -I$(ERIZO_HOME)/src/erizo 
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