debug:
	g++ -g -std=c++11 *.cpp -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -lopencv_core
release:
	g++ -std=c++11 -O3 *.cpp -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -lopencv_core
