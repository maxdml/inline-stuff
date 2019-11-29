import sys
import matplotlib
matplotlib.use('Agg')

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

plt.close('all')
#f = 'csv/single_l2_len_full.csv'
f = sys.argv[2]
df = pd.read_csv(f, delimiter='\t')
cols = ['L2_REFS', 'L2_MISSES', 'L2_DD_HITS', 'L2_DD_MISSES', 'L2_PF_HITS', 'L2_PF_MISSES', 'L3_REFS', 'L2_ALL_RFO']#, 'L2_HITS', 'L2_MISSES']#, 'CPU_CYCLES', 'INSN_RETIRED']
plt.figure(0, figsize=(10, 10))
fig, axs = plt.subplots(1, len(cols), squeeze=False, sharey='row', sharex=True, num=0)
for i, col in enumerate(cols):
        sns.scatterplot(x=df.index, y=col, data=df, ax=axs[0][i], label=col)
plt.legend()
#plt.savefig("plots/single_l2_len_full.pdf")
plt.savefig(sys.argv[1])
#plt.show()
