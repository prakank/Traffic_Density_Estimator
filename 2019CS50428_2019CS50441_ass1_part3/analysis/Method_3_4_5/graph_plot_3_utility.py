import pandas as pd
import os
import matplotlib.pyplot as plt

# os.chdir("/home/prakank/Sem4/COP290/Task1/Final")

df = pd.read_csv("Error_Utility_4.csv")
df.sort_values(by=['Method', ' Threads'], ascending=[True, True], inplace = True)
df = df.reset_index()
df.drop("index", axis=1, inplace = True)
for i  in df.index:
    df.loc[i].iloc[1] = int(df.loc[i].iloc[1])
    df.loc[i].iloc[2] = int(df.loc[i].iloc[2])
    df.loc[i].iloc[3] = float(df.loc[i].iloc[3])
    if(df.loc[i].iloc[4] == "inf"):
        df.loc[i].iloc[4] = 0
    else:
        df.loc[i].iloc[4] = float(df.loc[i].iloc[4])

df_error = df.groupby(["Method", " Threads"])[' Error'].transform('mean')
df_utlity = df.groupby(["Method", " Threads"])[' Utility'].transform('mean')
df_runtime = df.groupby(["Method", " Threads"])[' Runtime'].transform('mean')


Baseline_Error = 0
Baseline_Runtime = 0
M3_utility = []
M3_runtime = []
Thread = []
cnt = 0
L = [0 for i in range (100)]
for i in df.index:
    if(df.loc[i].iloc[0] == 0):
        Baseline_Runtime = df_runtime[i]
    if(df.loc[i].iloc[0] == 4):        
        if(L[int(df.loc[i].iloc[1])] == 1):
            continue
        else:            
            M3_utility.append(df_utlity[i])
            M3_runtime.append(df_runtime[i])
            Thread.append(int(df.loc[i].iloc[1]))
            L[int(df.loc[i].iloc[1])] = 1        
M3_utility.append(0)
M3_runtime.append(Baseline_Runtime)

fig,ax = plt.subplots()
ax.scatter(M3_utility, M3_runtime)
fig.suptitle("Runtime vs Utility - Method 4 (Spatial Distribution)", fontsize = 16)
ax.set_xlabel("Utility",fontsize=16,labelpad=2)
ax.set_ylabel("Runtime (in seconds)",fontsize=16,labelpad=10)
for i in range(len(Thread)):
    if(Thread[i]==1):
        ax.annotate("Thread:" + str(Thread[i]), xy = (M3_utility[i],M3_runtime[i]), xytext = (M3_utility[i]+1,M3_runtime[i]-10), arrowprops = dict(facecolor ='green', shrink = 0.05))
    else:
        ax.annotate("Threads:" + str(Thread[i]), xy = (M3_utility[i],M3_runtime[i]), xytext = (M3_utility[i]+1,M3_runtime[i]+10), arrowprops = dict(facecolor ='green', shrink = 0.05))
ax.annotate("Baseline", xy = (M3_utility[len(M3_utility)-1], M3_runtime[len(M3_runtime)-1]), xytext =(M3_utility[len(M3_utility)-1]+1, M3_runtime[len(M3_runtime)-1]+1), arrowprops = dict(facecolor ='green', shrink = 0.05))

plt.savefig('Method4_Utility.png')
plt.show()
