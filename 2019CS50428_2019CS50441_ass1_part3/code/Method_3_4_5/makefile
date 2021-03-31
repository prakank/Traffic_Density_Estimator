CC = g++
TARGET = method5
CFLAGS = -pthread -std=c++11 `pkg-config --cflags --libs opencv`

all: $(TARGET).cpp
	$(CC) $(TARGET).cpp -o $(TARGET) $(CFLAGS)


build:$(TARGET).cpp
	$(CC) $(TARGET).cpp -o $(TARGET) $(CFLAGS)
	./$(TARGET) trafficvideo.mp4 

	
plot: graph_plot.py
	python3 graph_plot.py
	
plot5: graph_plot_5.py
	python3 graph_plot_5.py
	
clean:
	rm -rf $(TARGET)
	rm -rf saved_figure.png
	rm -rf Density_Values.csv
	-rm -rf Density_vs_time.csv
