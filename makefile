all: debug

debug:
	g++ -g -I eyeLike/src -std=c++11 *.cpp -lopencv_core -lopencv_highgui -lopencv_objdetect -lopencv_imgproc -lopencv_videoio
release:
	g++ -I eyeLike/src -std=c++11 -O3 *.cpp -lopencv_core -lopencv_core -lopencv_objdetect -lopencv_imgproc -lopencv_videoio

