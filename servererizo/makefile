CC=gcc
CXX=g++

#基于erizo的rtc转发服务器。
CFLAGS = -I$(BUILD_DIR)/libdeps/build/include -I$(ERIZO_HOME)/src/third_party/webrtc/src -I$(ERIZO_HOME)/src/erizo -I$(SERVER_HOME)
CPPFLAGS = -std=c++11 
objects= KKBase.o    KKMutex.o    KKCond_t.o     KKRWLock.o \
	     kkAddr.o    kksocket.o   defkkkcp.o     ikcp.o \
		 KKKcp.o \
		 RtcConmgr.o servermain.o
	
LDFLAGS   = -static -std=c++11 -fPIC -static-libgcc -lboost_system
ObjTARGET = servererizo

$(ObjTARGET):$(objects)
	$(CXX) $(LDFLAGS) -o $(ObjTARGET) $(objects) \
	$(LDFLAGS)
	
KKBase.o:  ./kkRes/Base/KKBase.h ./kkRes/Base/KKBase.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) ./kkRes/Base/KKBase.cpp
KKMutex.o: ./kkRes/Base/KKMutex.h ./kkRes/Base/KKMutex.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) ./kkRes/Base/KKMutex.cpp
KKCond_t.o: ./kkRes/Base/KKMutex.h ./kkRes/Base/KKCond_t.h ./kkRes/Base/KKCond_t.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) ./kkRes/Base/KKCond_t.cpp
KKRWLock.o: ./kkRes/Base/KKRWLock.h ./kkRes/Base/KKRWLock.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) ./kkRes/Base/KKRWLock.cpp
kkAddr.o: ./kkRes/socket/kkAddr.h ./kkRes/socket/kkAddr.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) ./kkRes/socket/kkAddr.cpp
kksocket.o: ./kkRes/socket/kksocket.h ./kkRes/socket/kksocket.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) ./kkRes/socket/kksocket.cpp
defkkkcp.o: ./KKkcp/defkkkcp.h ./KKkcp/defkkkcp.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS)  ./KKkcp/defkkkcp.cpp
ikcp.o: ./KKkcp/ikcp.h ./KKkcp/ikcp.c
	$(CC) -c ./KKkcp/ikcp.c
KKKcp.o: ./KKkcp/KKTwoKeyMap.h ./KKkcp/KKKcp.h ./KKkcp/KKKcp.cpp

RtcConmgr.o:  RtcConmgr.h RtcConmgr.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS)  RtcConmgr.cpp
servermain: stdafx.h servermain.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS)  servermain.cpp
install:
	cp -f $(ObjTARGET) ./
clean:
	rm $(ObjTARGET) $(objects)