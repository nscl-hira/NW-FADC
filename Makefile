all:
	rootcint -f DataStructDict.cxx -c DataStruct.h DataStructLinkDef.h
	g++ -fPIC -c DataStructDict.cxx `root-config --cflags`
	g++ -fPIC -c DataStruct.cc `root-config --cflags`
	g++ --shared -o DataStruct.so `root-config --libs` DataStruct.o DataStructDict.o
	g++ -o FADC_Conversion_root FADC_Conversion_root_v1.cc `root-config --cflags --libs` DataStruct.o DataStructDict.o
	g++ -o TCB_Conversion_root TCB_Conversion_root_v1.cc `root-config --cflags --libs` DataStruct.o DataStructDict.o

clean:
	rm -rf *.so *.o *.pcm *Dict.cxx FADC_Conversion_root TCB_Conversion_root
