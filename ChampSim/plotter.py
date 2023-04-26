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
# (python3 datacollector.py or ./datacollector.py) predname1-instr1 predname2-instr2 ... metric
# or
#  (python3 datacollector.py or ./datacollector.py) predname1-instr1 metric

def to_metric(inp:str)->str:
    for name in columnnames:
        if name.lower() == inp.lower():
            return name
    else: 
        print("Metric?")
        exit(-1)


def cmpplotter(datas,metric:str):
    datas.sort()
    name = lambda type:  f'{plotpath}{"_".join(datas)}_{metric}_{type}.png'
    #assert (sum(x!=y for x,y in zip(data1.split('-'),data2.split('-')))==1),"Why comparing apples and oranges?"
    dfs = [pd.read_csv(f'{csv_dir}{data}.csv').sort_values('tracename') for data in datas]
    
    width = 1/(len(dfs)+0.5)
    x_labels = dfs[0]['tracename']
    xax = np.arange(len(x_labels))
    y_values = [df[metric] for df in dfs]
    #bar
    fig, ax = plt.subplots()
    for i, (y , label) in enumerate(zip(y_values,datas)):
        ax.bar(xax + i*width, y,width=width, label=label)
    ax.set_xticks(xax + width / 2)
    ax.set_xticklabels(x_labels,rotation=90,fontsize=8)
    ax.set_xlabel('Trace')
    ax.set_ylabel(metric)
    ax.set_title(" v/s ".join(datas))
    ax.legend()
    plt.tight_layout()
    plt.savefig(name('bar'))
    plt.clf()
    # line
    for y,label in zip(y_values,datas):
        plt.plot(x_labels,y,label=label,marker='o')
    plt.title(" v/s ".join(datas))
    plt.xticks(rotation=90, ha='right',fontsize=8)
    plt.xlabel(metric)
    plt.ylabel('Trace')
    plt.legend()
    plt.tight_layout()
    plt.savefig(name('line'))
    print("Done")
    return 


def singplotter(data:str,metric:str):
    name = lambda type: f'{plotpath}{data}_{metric}_{type}.png'
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
    plt.savefig(name('bar'))
    
    plt.clf()

    plt.plot(x,y,marker='o')
    plt.xlabel('Trace')
    plt.ylabel(metric)
    plt.title(data)
    plt.xticks(rotation=90, ha='right',fontsize=8)
    plt.tight_layout()
    plt.savefig(name('line'))

    print("Done")
    return

def main():
    output_dir = os.path.dirname(plotpath)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
        
    if len(sys.argv)==3:
        data = sys.argv[1]
        metric = to_metric(sys.argv[2])
        singplotter(data,metric)
    else:
        datas = sys.argv[1:-1]
        metric = to_metric(sys.argv[-1])
        cmpplotter(datas,metric)
    
    

    

if __name__=="__main__":
    main()