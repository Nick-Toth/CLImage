OCV_LINK = -I/usr/local/Cellar/opencv/2.4.13.2/include/ -L/usr/local/opt/opencv@2/lib -I/usr/local/opt/opencv@2/include -lopencv_core -lopencv_imgproc -lopencv_highgui

CFLAGS =  -g -Wall -ansi

Test:
	g++ ImgTest.cpp Image.o $(OCV_LINK) $(CFLAGS)

Image:
	g++ Image.cpp -c $(OCV_LINK) $(CFLAGS)
