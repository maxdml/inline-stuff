import sys
import datetime
import matplotlib
matplotlib.use('Agg')

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

plt.close('all')
#f = 'csv/single_l2_len_full.csv'
f = sys.argv[2]
df = pd.read_csv(f, delimiter='\t')
df.TIME -= min(df.TIME)
cols = ['L2_REFS', 'L2_MISSES', 'L2_RFO_HITS', 'L2_RFO_MISSES', 'OFFCORE_RFO', 'L3_REFS'] #, 'L2_PF_HITS', 'L2_PF_MISSES']#, 'L2_HITS', 'L2_MISSES']#, 'CPU_CYCLES', 'INSN_RETIRED']
plt.figure(0, figsize=(10, 10))
fig, axs = plt.subplots(1, len(cols), squeeze=False, sharey='row', sharex=True, num=0)
for i, col in enumerate(cols):
       sns.scatterplot(x=df.TIME, y=col, data=df, ax=axs[0][i], label=col, hue='TID')
       #sns.scatterplot(x, y=col, data=df, ax=axs[0][i], label=col)
plt.legend()
#plt.savefig("plots/single_l2_len_full.pdf")
plt.savefig(sys.argv[1])
#plt.show()
