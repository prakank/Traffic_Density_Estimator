CC = g++
TARGET3 = method3
TARGET4 = method4
TARGET5 = method5
CFLAGS = -pthread -std=c++11 `pkg-config --cflags --libs opencv`

all: $(TARGET3).cpp
	$(CC) $(TARGET3).cpp -o $(TARGET3) $(CFLAGS)
	$(CC) $(TARGET4).cpp -o $(TARGET4) $(CFLAGS)
	$(CC) $(TARGET5).cpp -o $(TARGET5) $(CFLAGS)
	


build:$(TARGET).cpp
	$(CC) $(TARGET).cpp -o $(TARGET) $(CFLAGS)
	./$(TARGET)

	
plot: graph_plot.py 
	python3 graph_plot.py

plot3: graph_plot_3.py
	python3 graph_plot_3.py

plot4: graph_plot_4.py
	python3 graph_plot_4.py

plot5: graph_plot_5.py
	python3 graph_plot_5.py
	
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

