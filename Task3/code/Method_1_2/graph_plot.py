import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

#plt.figure(figsize = (20, 20))

df1 = pd.read_csv("out1.csv")
df2 = pd.read_csv("out2.csv")


plt.plot(df1["Utility"], df1['Runtime(in milliseconds)'])

for i in range (df1["Utility"].size):
    plt.annotate("x = " + str(df1["Parameter x(Interval of sub-sampling)"][i]), (df1["Utility"][i], df1["Runtime(in milliseconds)"][i]), fontsize = 7)


plt.xlabel('Utility')
plt.ylabel('Runtime(in milliseconds)')


plt.savefig('method1.png')

plt.show()


plt.plot(df2["Utility"], df2['Runtime(in milliseconds)'])

for i in range (df2["Utility"].size):
    plt.annotate("res = " + str(df2["Parameter res(Resolution in p)"][i]) + "p", (df2["Utility"][i], df2["Runtime(in milliseconds)"][i]), fontsize = 7)


plt.xlabel('Utility')
plt.ylabel('Runtime(in milliseconds)')


plt.savefig('method2.png')

plt.show()

