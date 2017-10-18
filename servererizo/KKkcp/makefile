#cd D:/work/Projects/kkres/KKkcp
CC=gcc
CXX=g++

BASE_PATH=D:/msys32
CFLAGS = -I$(BASE_PATH)/usr/include  
CPPFLAGS = -std=c++11
objects=KKBase.o KKMutex.o KKCond_t.o  KKRWLock.o kkAddr.o kksocket.o \
	defkkkcp.o ikcp.o KKTwoKeyMap.o KKKcp.o \
	TranServer.o
	
LDFLAGS   =  -shared -std=c++11 -fPIC
ObjTARGET = KKkcp.so

$(ObjTARGET):$(objects)
	$(CXX) $(LDFLAGS) -o $(ObjTARGET) $(objects) \
	$(LDFLAGS) 
KKBase.o: ../kkRes/Base/KKBase.h ../kkRes/Base/KKBase.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS)  ../kkRes/Base/KKBase.cpp
KKMutex.o: ../kkRes/Base/KKMutex.h ../kkRes/Base/KKMutex.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) ../kkRes/Base/KKMutex.cpp
KKCond_t.o: ../kkRes/Base/KKMutex.h ../kkRes/Base/KKCond_t.h ../kkRes/Base/KKCond_t.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) ../kkRes/Base/KKCond_t.cpp
KKRWLock.o: ../kkRes/Base/KKRWLock.h ../kkRes/Base/KKRWLock.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) ../kkRes/Base/KKRWLock.cpp
kkAddr.o: ../kkRes/socket/kkAddr.h ../kkRes/socket/kkAddr.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS)  ../kkRes/socket/kkAddr.cpp
kksocket.o: ../kkRes/socket/kksocket.h ../kkRes/socket/kksocket.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS)  ../kkRes/socket/kksocket.cpp
defkkkcp.o: defkkkcp.h defkkkcp.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS)  defkkkcp.cpp
ikcp.o: ikcp.h ikcp.c
	$(CC) -c ikcp.c
KKKcp.o: KKTwoKeyMap.h KKKcp.h KKKcp.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) KKKcp.cpp
TranServer.o: TranServer.h TranServer.cpp
	$(CXX) -c $(CFLAGS) $(CPPFLAGS) TranServer.cpp
install:
	cp -f $(ObjTARGET) ./
clean:
	rm $(ObjTARGET) $(objects)