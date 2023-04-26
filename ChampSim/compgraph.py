import re
import numpy as np
import sys
import matplotlib.pyplot as plt
from datacollector import tracenames
# Input text string

# Define regex pattern to match array lines
pattern = r'\[(.*?)\]$'
predname = sys.argv[1]
instr = int(sys.argv[2])
txt_output_path = './results/txtdata/'
plotpath = './results/plots/'

def file_to_array(fname:str)->np.array:
    with open(fname,'r') as f:
        input_text = f.read()
    # Extract last array line using regex
    match = re.search(pattern, input_text, re.MULTILINE)
    if match:
        last_array_line = match.group(1)
        # Convert extracted string to list of integers
        last_array = list(map(int, last_array_line.split(', ')))
    else:
        print("No match found.")
        exit(-1)
    return np.array(last_array)

def plot():

    for name in tracenames():
        plt.plot(file_to_array(f'{txt_output_path}{name}-{instr}M-{predname}.txt'),label=name)
    plt.title(f"Component Usage for {predname} ({instr}M)")
    plt.legend()
    plt.savefig(f'{plotpath}{predname}-{instr}M-component.png',dpi=300)
    pass

if __name__=="__main__":
    plot()