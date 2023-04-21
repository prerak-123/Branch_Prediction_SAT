#!/bin/bash

./config.sh champsim_config.json
make

for f in traces/*
do
    bin/champsim --warmup_instructions 50000000 --simulation_instructions 50000000 $f > $f-50M-ltage.txt
done

# bin/champsim --warmup_instructions 50000000 --simulation_instructions 50000000 ./traces/cadical-low-60K-1186B.champsimtrace.xz > results/ltage_50M_cadical-low-60K-1186B.champsimtrace.xz.txt

# bin/champsim --warmup_instructions 50000000 --simulation_instructions 50000000 ./traces/cadical-med-30K-109B.champsimtrace.xz > results/ltage_50M_cadical-med-30K-109B.champsimtrace.xz.txt

# bin/champsim --warmup_instructions 50000000 --simulation_instructions 50000000 ./traces/cadical-med-30K-137B.champsimtrace.xz > results/ltage_50M_cadical-med-30K-137B.champsimtrace.xz.txt

# bin/champsim --warmup_instructions 50000000 --simulation_instructions 50000000 ./traces/cadical-med-30K-267B.champsimtrace.xz > results/ltage_50M_cadical-med-30K-267B.champsimtrace.xz.txt

# bin/champsim --warmup_instructions 50000000 --simulation_instructions 50000000 ./traces/cadical-med-30K-831B.champsimtrace.xz > results/ltage_50M_cadical-med-30K-831B.champsimtrace.xz.txt

# bin/champsim --warmup_instructions 50000000 --simulation_instructions 50000000 ./traces/cadical-med-30K-1246B.champsimtrace.xz > results/ltage_50M_cadical-med-30K-1246B.champsimtrace.xz.txt

# bin/champsim --warmup_instructions 50000000 --simulation_instructions 50000000 ./traces/cadical-med-30K-1463B.champsimtrace.xz > results/ltage_50M_cadical-med-30K-1463B.champsimtrace.xz.txt

# bin/champsim --warmup_instructions 50000000 --simulation_instructions 50000000 ./traces/kissat-mab-hywalk-low-30K-299B.champsimtrace.xz > results/ltage_50M_kissat-mab-hywalk-low-30K-299B.champsimtrace.xz

sed -i 's/ltage/hashed_perceptron/g' champsim_config.json

./config.sh champsim_config.json
make

for f in traces/*
do
    bin/champsim --warmup_instructions 50000000 --simulation_instructions 50000000 $f > $f-50M-hashed_perceptron.txt
done

