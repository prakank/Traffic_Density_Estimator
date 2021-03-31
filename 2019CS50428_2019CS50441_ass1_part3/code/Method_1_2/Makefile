CC = g++
TARGET1 = method1
TARGET2 = method2
CFLAGS = -pthread -std=c++11 `pkg-config --cflags --libs opencv`

all: $(TARGET1).cpp $(TARGET2).cpp
	$(CC) $(TARGET1).cpp -o $(TARGET1) $(CFLAGS)
	$(CC) $(TARGET2).cpp -o $(TARGET2) $(CFLAGS)

	
plot: graph_plot.py
	python3 graph_plot.py


clean:
	-rm $(TARGET1)
	-rm $(TARGET2)
	-rm out1.csv
	-rm out2.csv
	-rm method1.png
	-rm method2.png

	
