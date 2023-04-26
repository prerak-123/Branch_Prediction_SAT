# Branch Prediction for SAT

All code is present in the _ChampSim_ folder. This is the version of ChampSim used in the lab.

Instructions:
For the scripts traces should be inside ChampSim/traces (alternatively change the variable values in the scripts).

The different versions of branch predictors are in the ChampSim/branch folder. 

### Predictors
- ltage_base : Our final implementation of LTAGE based on the paper provided to us.

- ltage_nolp : A simpler implementation of TAGE without the loop predictor.

- ltage_p4 : LTAGE with the number of prediction bits increased to 4.

- ltage_byuseful : LTAGE with the component being chosen according to useful counter instead of one taking longest history

- ltage_diff_num_entries : LTAGE but reshuffling the number of entries so that components with more access is given more entries.

- ltage_crazy : LTAGE but with base predictor as a hashed perceptron

### Data
Data generated for all the traces and used in the report, and otherwise can be found in the _results_ folder. Comparisons between different versions of LTAGE can be found in the form of plots and tables.


### Scripts
The scripts here are - 
- datacollector.py - args <predictor_name> <no_of_instr> <overwrite> \<overwrite> (1 to redo sims/ optional) .

This will run the simulation if necessary, and then geenrate a csv from the txt outputs. Paths are within traces by default.

- plotter.py - args \<predictor1-instr1M> ... \<predictorn-instrnM> \<metric>

This will generate bar/line plots comparing these for the same metric.

- table.py - args nothing or  \<base_for_comparison> \<predictor1> ... \<predictorn> \<instr> \<metric> 

This will generate a table comparing the ratio difference in the metric across predictors for each trace ,and also save its csv,

- rename.py - no args

Just a utility to rename results

