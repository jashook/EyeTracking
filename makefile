debug:
	g++ *.cpp -std=c++11 -g -lopencv_core -lopencv_highgui -lopencv_objdetect -lopencv_imgproc
release:
	g++ *.cpp -std=c++11 -O3 -lopencv_core -lopencv_highgui -lopencv_objdetect -lopencv_imgproc
