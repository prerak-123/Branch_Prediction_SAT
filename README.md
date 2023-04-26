# Branch Prediction for SAT

All code is present in the _ChampSim_ folder. This is the version of ChampSim used in the lab.

Instructions:
For the scripts traces should be inside ChampSim/traces (alternatively change the variable values in the scripts).

The different versions of branch predictors are in the ChampSim/branch folder. 


#### Data
Data used in the report, and otherwise can be found in - <fill this>


#### Scripts
The scripts here are - 
- datacollector.py - args <predictor_name> <no_of_instr> <overwrite>(1 to overwrite existing simulation data/ optional) .

This will run the simulation if necessary, and then geenrate a csv from the txt outputs. Paths are within traces by default.

- plotter.py - args \<predictor1-instr1M> ... \<predictorn-instrnM> \<metric>

This will generate bar/line plots comparing these for the same metric.

- table.py - args nothing or  \<base_for_comparison> \<predictor1> ... \<predictorn> \<instr> \<metric> 

This will generate a table comparing the ratio difference in the metric across predictors for each trace ,and also save its csv,

