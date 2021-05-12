import pandas as pd
import matplotlib.pyplot as plt

xco = []
for i in range(1147):
    xco.append(i*382/1147)
df1 = pd.read_csv("Density_Values_M4.csv")
df2 = pd.read_csv("Density_Values_Baseline_Queue.csv")

runtime = df1.loc[0]
df1 = df1.drop(0)
df1.drop("FrameNumber", axis=1, inplace = True)
df1.reset_index(inplace = True)
df1.drop("index", axis=1, inplace = True)
threads = runtime.iloc[0]
runtime_thread = runtime.iloc[1].split("_")[0]
runtime_baseline = runtime.iloc[1].split("_")[1]

for i in df1.index:
    df1.loc[i].iloc[0] = float(df1.loc[i].iloc[0]) 

plt.plot( xco, df1["Density Values"], label='Queue Density with threads = ' + str(threads))
plt.plot( xco, df2["Density Values"], label = 'Baseline Queue Density')

plt.xlabel('Time(in Seconds)\nRuntime with threads = ' + str(threads) + ": " + str(runtime_thread) + " seconds\nBaseline Runtime: " + str(runtime_baseline) + " seconds", fontsize =12)
plt.ylabel('Density', fontsize =16)
plt.title('Comparsion plot of Queue Density (Threads vs Baseline)')
plt.axvline(100, 0, 1, color='g')
plt.axvline(185,0,1,color = 'r')
plt.axvline(275, 0, 1, color = 'g')
plt.axvline(350, 0, 1, color = 'r')
plt.legend()
plt.savefig("Method_4_TemporallyDistributed.png")

plt.show()

### ERROR computed for FramesToSkip = 5 is: 