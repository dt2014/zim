import sys
import pandas as pd
import matplotlib.pyplot as plt

fig = plt.figure()

figures = pd.read_csv(sys.argv[1], index_col = [0])

#yticks = [-2500, 0, 2500, 5000, 7500, 10000, 12500, 15000, 17500, 20000, 22500, 25000, 27500]
figures.plot(title = sys.argv[2])

plt.savefig(sys.argv[3], dpi=400, bbox_inches='tight')