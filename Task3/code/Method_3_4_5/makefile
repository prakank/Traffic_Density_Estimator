CC = g++
TARGET3 = method3
TARGET4 = method4
TARGET5 = method5
CFLAGS = -pthread -std=c++11 `pkg-config --cflags --libs opencv`

all: $(TARGET3).cpp
	$(CC) $(TARGET3).cpp -o $(TARGET3) $(CFLAGS)
	$(CC) $(TARGET4).cpp -o $(TARGET4) $(CFLAGS)
	$(CC) $(TARGET5).cpp -o $(TARGET5) $(CFLAGS)
	
all3: $(TARGET3).cpp
	$(CC) $(TARGET3).cpp -o $(TARGET3) $(CFLAGS)
	./method3 trafficvideo.mp4 1 
	python3 graph_plot_3.py
	./method3 trafficvideo.mp4 2 
	python3 graph_plot_3.py
	./method3 trafficvideo.mp4 3 
	python3 graph_plot_3.py
	./method3 trafficvideo.mp4 4 
	python3 graph_plot_3.py
	./method3 trafficvideo.mp4 5 
	python3 graph_plot_3.py
	./method3 trafficvideo.mp4 6 
	python3 graph_plot_3.py
	./method3 trafficvideo.mp4 7 
	python3 graph_plot_3.py
	./method3 trafficvideo.mp4 8 
	python3 graph_plot_3.py
	python3 graph_plot_3_utility.py

all4: $(TARGET4).cpp
	./method4 trafficvideo.mp4 1
	python3 graph_plot_4.py
	./method4 trafficvideo.mp4 2
	python3 graph_plot_4.py
	./method4 trafficvideo.mp4 3
	python3 graph_plot_4.py
	./method4 trafficvideo.mp4 4
	python3 graph_plot_4.py
	./method4 trafficvideo.mp4 5
	python3 graph_plot_4.py
	./method4 trafficvideo.mp4 6
	python3 graph_plot_4.py
	./method4 trafficvideo.mp4 7
	python3 graph_plot_4.py
	./method4 trafficvideo.mp4 8
	python3 graph_plot_4.py
	python3 graph_plot_4_utility.py


build:$(TARGET).cpp
	$(CC) $(TARGET).cpp -o $(TARGET) $(CFLAGS)
	./$(TARGET)

plot3: graph_plot_3.py
	python3 graph_plot_3.py

plot4: graph_plot_4.py
	python3 graph_plot_4.py

plot5: graph_plot_5.py
	python3 graph_plot_5.py

plot3_utility: graph_plot_3_utility.py
	python3 graph_plot_3_utility.py

plot4_utility: graph_plot_4_utility.py
	python3 graph_plot_4_utility.py	

clean:
	rm -rf $(TARGET3)
	rm -rf $(TARGET4)
	rm -rf $(TARGET5)
	rm -rf Method_5_Sparse_vs_Dense.png
	rm -rf Method_4_TemporallyDistributed.png
	rm -rf Method_3_SpatiallyDistributed.png
	rm -rf Density_Values_Baseline_Dynamic.csv
	rm -rf Density_Values_Baseline_Queue.csv
	rm -rf Density_Values_M3.csv
	rm -rf Density_Values_M4.csv
	rm -rf Density_Values_M5.csv
	rm -rf Method3_Utility.png
	rm -rf Method4_Utility.png
	rm -rf Error_Utility_3.csv
	rm -rf Error_Utility_4.csv
