all: debug

debug:
	g++ -g -std=c++11 *.cpp -lopencv_core -lopencv_highgui -lopencv_objdetect -lopencv_imgproc
release:
	g++ -std=c++11 -O3 *.cpp -lopencv_core -lopencv_core -lopencv_objdetect -lopencv_imgproc
