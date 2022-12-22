#parallel task bench results plotter

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sbrn
import pandas as pd
from matplotlib.colors import LogNorm
import sys

sbrn.set_context("paper")
_fontsize = 13
sbrn.set_style("whitegrid")
sbrn.set_style("ticks", {"xtick.major.size": 12, "ytick.major.size": 12})

dataSet = pd.read_table(sys.argv[1], skiprows = 0, header=0, delimiter=",")
print(dataSet.head())

ax = sbrn.lineplot(x="size(MB)", y="gups",  hue="num_threads",  data=dataSet, palette="tab20")

#ax.set_xscale("log")
ax.set_yscale("log")
ax.set_xlabel('N',fontsize=_fontsize);
ax.set_ylabel('t(sec)',fontsize=_fontsize);

ax.legend(fontsize=_fontsize)
ax.set_title(sys.argv[1],fontsize=_fontsize)

ax.set_ylim(top=4, bottom=0.00005)

#plt.tight_layout()
#plt.show()
plt.savefig("./PNG/"+sys.argv[1]+".png", dpi=300)
plt.savefig("./SVG/"+sys.argv[1]+".svg", dpi=300)
