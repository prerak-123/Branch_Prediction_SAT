#!/usr/bin/python3
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import os
import sys

columnnames = ('predictor','instructions','tracename',
    'IPC','Branch_Prediction_Accuracy','MPKI','ROB_Occ_at_Mispredict'
    'BRANCH_DIRECT_JUMP','BRANCH_INDIRECT','BRANCH_CONDITIONAL','BRANCH_DIRECT_CALL','BRANCH_INDIRECT_CALL', 'BRANCH_RETURN')
plotpath = './traces/plots/'
csv_dir = './traces/csvdata/'
# usage 
# (python3 datacollector.py or ./datacollector.py) predname1-instr1 predname2-instr2 metric
# or
#  (python3 datacollector.py or ./datacollector.py) predname1-instr1 metric

def to_metric(inp:str)->str:
    for name in columnnames:
        if name.lower() == inp.lower():
            return name
    else: 
        print("Metric?")
        exit(-1)


def cmpplotter(data1:str,data2:str,metric:str):
    assert (sum(x!=y for x,y in zip(data1.split('-'),data2.split('-')))==1),"Why comparing apples and oranges?"
    df1 = pd.read_csv(f'{csv_dir}{data1}.csv')
    df2 = pd.read_csv(f'{csv_dir}{data2}.csv')
    df1 = df1.sort_values('tracename')
    df2 = df2.sort_values('tracename')
    width = 0.35
    x_labels = df1['tracename']
    xax = np.arange(len(x_labels))
    y_values1 = df1[metric]
    y_values2 = df2[metric]
    fig, ax = plt.subplots()
    ax.bar(xax, y_values1,width=width, label=data1)
    ax.bar(xax+width , y_values2,width=width, label=data2)
    ax.set_xticks(xax + width / 2)
    ax.set_xticklabels(x_labels,rotation=90,fontsize=8)
    ax.set_xlabel('Trace')
    ax.set_ylabel(metric)
    ax.set_title(f'{data1} vs {data2}')
    ax.legend()
    plt.tight_layout()
    print("Done")
    return plt.gcf()


def singplotter(data:str,metric:str):
    csv_output_path = f'{csv_dir}{data}.csv'

    df = pd.read_csv(csv_output_path)
    df = df.sort_values('tracename')
    x = df['tracename']
    y = df[metric]
    plt.bar(x,y)
    plt.xlabel('Trace')
    plt.ylabel(metric)
    plt.title(data)
    plt.xticks(rotation=90, ha='right',fontsize=8)
    plt.tight_layout()
    print("Done")
    return plt.gcf()

def main():
    if len(sys.argv)==4:
        data1 = sys.argv[1]
        data2 = sys.argv[2]
        metric = to_metric(sys.argv[3])        
        plotter = cmpplotter(data1,data2,metric)
        if data1>data2 : data1,data2 = data2,data1
        name = f'{plotpath}{data1}_{data2}_{metric}.png'
    elif len(sys.argv)==3:
        data = sys.argv[1]
        metric = to_metric(sys.argv[2])
        plotter = singplotter(data,metric)
        name = f'{plotpath}{data}-{metric}.png'
    else:
        print("incorrect usage")
    
    output_dir = os.path.dirname(plotpath)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    plotter.savefig(name)
    

if __name__=="__main__":
    main()