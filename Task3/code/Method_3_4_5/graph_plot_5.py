import pandas as pd
import matplotlib.pyplot as plt

xco = []
for i in range(1147):
    xco.append(i*382/1147)
df1 = pd.read_csv("Density_Values_Baseline_Dynamic.csv")
df2 = pd.read_csv("Density_Values_M5.csv")
plt.plot(xco, df1["Density Values"])
plt.plot(xco, df2["Density Values"]*20)
plt.xlabel("Time (in seconds)")
plt.ylabel("Density Values")
plt.title("Comparison between Sparse and Dense Optical Flow")
plt.savefig("Method_5_Sparse_vs_Dense.png")
plt.show()

### ERROR computed for FramesToSkip = 5 is: 