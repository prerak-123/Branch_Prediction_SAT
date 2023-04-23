#!/usr/bin/python3

import sys
import pandas as pd
import os
import multiprocessing
import subprocess

# python3 datacollector.py branch-predictor-file num-instr-in-million recollect-data(bool)
#  or
# ./datacollector.py branch-predictor-file num-instr-in-million recollect-data(bool)
predname = sys.argv[1]
instr = int(sys.argv[2])
try:
    overrwrite = bool(int(sys.argv[3]))
except:
    overrwrite = False

csv_output_path = f'./traces/csvdata/{predname}-{instr}M.csv'
traceinppath = './traces/'
txt_output_path = './traces/txtdata/'

def run_command(command):
    subprocess.run(command, shell=True)
# for the csv
def write_to_csv(df : pd.DataFrame):
    # Create the directory if it doesn't exist
    output_dir = os.path.dirname(csv_output_path)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    # Write DataFrame to CSV
    df.to_csv(csv_output_path, index=False)

# just gets the trace names
def tracenames() -> list[str]:
    tracenames = []
    for __, _, files in os.walk(traceinppath):
        for filename in files:
            # Check if the filename contains the keyword
            if 'high' not in filename and filename.endswith('.xz'):
                # Append the filename to the list
                tracenames.append(filename.split('.')[0])
    return tracenames

## runs simulations on traces in that folder
def do_sims():
    # a very insecure way of doing this
    if os.path.exists('./run_champsim.sh'):
        return do_sims_oldcs()
    config_file_name = f'champsim_config_{predname}.json'
    if not os.path.exists(f'./{config_file_name}'):
        import json
        with open('champsim_config.json','r') as f:
            data = json.load(f)
        d1 = data['ooo_cpu'][0]
        d1['branch_predictor'] = predname
        with open(config_file_name, 'w') as file:
            json.dump(data, file, indent=4)

    names = tracenames()

    os.system(f'./config.sh {config_file_name}')
    os.system('make')

    output_dir = os.path.dirname(txt_output_path)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    processes = []
    for name in names:
        cmd = f'bin/champsim --warmup_instructions {int(instr*1000000)} --simulation_instructions {int(instr*1000000)} {traceinppath}{name}.champsimtrace.xz > {txt_output_path}{name}-{instr}M-{predname}.txt'
        p = multiprocessing.Process(target=run_command, args=(cmd,))
        p.start()
        processes.append(p)
    for p in processes:
        p.join()
    # done
# for old champsim
def do_sims_oldcs():
    output_dir = os.path.dirname(txt_output_path)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    ## do change this if reqd
    os.system(f'./build_champsim.sh {predname} no no no no lru 1 &> /dev/null')
    bin = f'{predname}-no-no-no-no-lru-1core'
    cmd2 = lambda name: f'./bin/{bin} -warmup_instructions {int(instr*1000000)} -simulation_instructions {int(instr*1000000)} -traces {traceinppath}{name}.champsimtrace.xz > {txt_output_path}{name}-{instr}M-{predname}.txt'
    processes = []
    names = tracenames()
    for name in names:
        p = multiprocessing.Process(target=run_command, args=(cmd2(name),))
        p.start()
        processes.append(p)
    for p in processes:
        p.join()

import re
columnnames = ('IPC','Branch_Prediction_Accuracy','MPKI','ROB_Occ_at_Mispredict')+(
    'BRANCH_DIRECT_JUMP','BRANCH_INDIRECT','BRANCH_CONDITIONAL','BRANCH_DIRECT_CALL','BRANCH_INDIRECT_CALL', 'BRANCH_RETURN')
regexes = (r"cumulative IPC:\s+([\d\.]+)",r"Branch Prediction Accuracy:\s+([\d\.]+)",r"MPKI:\s+([\d\.]+)",r"Average ROB Occupancy at Mispredict:\s+([\d\.]+)") + (
    r"BRANCH_DIRECT_JUMP:\s+([\d\.]+)",r"BRANCH_INDIRECT:\s+([\d\.]+)",r"BRANCH_CONDITIONAL:\s+([\d\.]+)",r"BRANCH_DIRECT_CALL:\s+([\d\.]+)",r"BRANCH_INDIRECT_CALL:\s+([\d\.]+)",r"BRANCH_RETURN:\s+([\d\.]+)")

def txt_to_csv():
    names = tracenames()
    #for name in names:
    #    df = pd.concat([df, txt_to_pd(name)], axis=0, ignore_index=True)
    df = pd.DataFrame([txt_to_pd(name) for name in names])   
    write_to_csv(df)

## this function is left to implement
def txt_to_pd(tracename:str)->dict:
    filename = f'{txt_output_path}{tracename}-{instr}M-{predname}.txt'
    entry = {'predictor':predname,'instructions': instr, 'tracename': tracename}
    with open(filename,'r') as simfile:
        text = simfile.read()
    for name,regex in zip(columnnames,regexes):
        match = re.search(regex,text)
        entry[name] = float(match.group(1))
    return entry

#### ok uncomment when add txt to pd works
def main():
    if overrwrite or not os.path.exists(f'{txt_output_path}{tracenames()[0]}-{instr}M-{predname}.txt'):
        print("Doing Sims")
        do_sims()
    print("Making csv")
    txt_to_csv()
        
    pass

if __name__=='__main__':
    main()
