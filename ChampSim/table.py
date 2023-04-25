#!/usr/bin/python3
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import os
import sys

# usage
# python3 table.py size metric 
# or 
# table.py baseline comparator-1 comparator-2 ...  size metric 

columnnames = ('predictor','instructions','tracename',
    'IPC','Branch_Prediction_Accuracy','MPKI','ROB_Occ_at_Mispredict'
    'BRANCH_DIRECT_JUMP','BRANCH_INDIRECT','BRANCH_CONDITIONAL','BRANCH_DIRECT_CALL','BRANCH_INDIRECT_CALL', 'BRANCH_RETURN')
plotpath = './traces/plots/'
csv_dir = './traces/csvdata/'

default_base = 'ltage'
default_comparators = ['hashed_perceptron','ltage'] # change this

def to_metric(inp:str)->str:
    for name in columnnames:
        if name.lower() == inp.lower():
            return name
    else: 
        print("Metric?")
        exit(-1)

def table_maker(base:str,comparators:list[str],size:int,metric:str):
    
    dfbase = pd.read_csv(f'{csv_dir}{base}-{size}M.csv').sort_values('tracename')
    tracenames = np.append(dfbase['tracename'].to_numpy(),"mean")
    basem = dfbase[metric].to_numpy()

    metrics = [pd.read_csv(f'{csv_dir}{cmp}-{size}M.csv').sort_values('tracename')[metric].to_numpy()/basem for cmp in comparators]
    put_gm = lambda arr : np.append(arr,np.power(np.prod(arr),1/(len(arr))))
    metrics = list(map(put_gm,metrics))

    data = {'Predictor':tracenames}
    for cmp,metrics in zip(comparators,metrics):
        data[cmp] = metrics
    # Create a colormap for coloring the cells
    cmap = plt.cm.Blues
    table = pd.DataFrame(data)

    # Create a table with colored cells
    fig, ax = plt.subplots(figsize=(6, 4))  # Set the figure size as per your requirement
    ax.axis('off')  # Turn off the axis
    table = ax.table(cellText=table.values, colLabels=table.columns, loc='center', cellLoc='center', colColours=[cmap(0.7)] * len(table.columns))
    comparators.sort()
    # Save the table as a PNG image
    plt.savefig(f'{plotpath}{"_".join(comparators)}_vs_{base}_{size}M_table.png', bbox_inches='tight',dpi=300)
    plt.close()
    return

def main():
    if len(sys.argv) == 3:
        base = default_base
        comparators = default_comparators
    else:
        base = sys.argv[1]
        comparators = sys.argv[2:-2]
    size = int(sys.argv[-2])
    metric = to_metric(sys.argv[-1])

    table_maker(base,comparators,size,metric)

if __name__=="__main__":
    main()
