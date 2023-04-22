./build_champsim.sh bimodal no no no no fifo 1
./build_champsim.sh bimodal no no no no lfu 1
./build_champsim.sh bimodal no no no no random 1
./run_champsim.sh bimodal-no-no-no-no-fifo-1core 20 40 400.perlbench-41B.champsimtrace.xz
./run_champsim.sh bimodal-no-no-no-no-lfu-1core 20 40 400.perlbench-41B.champsimtrace.xz
./run_champsim.sh bimodal-no-no-no-no-random-1core 20 40 400.perlbench-41B.champsimtrace.xz
./run_champsim.sh bimodal-no-no-no-no-fifo-1core 20 40 403.gcc-16B.champsimtrace.xz
./run_champsim.sh bimodal-no-no-no-no-lfu-1core 20 40 403.gcc-16B.champsimtrace.xz
./run_champsim.sh bimodal-no-no-no-no-random-1core 20 40 403.gcc-16B.champsimtrace.xz
./run_champsim.sh bimodal-no-no-no-no-fifo-1core 20 40 436.cactusADM-1804B.champsimtrace.xz
./run_champsim.sh bimodal-no-no-no-no-lfu-1core 20 40 436.cactusADM-1804B.champsimtrace.xz
./run_champsim.sh bimodal-no-no-no-no-random-1core 20 40 436.cactusADM-1804B.champsimtrace.xz
./run_champsim.sh bimodal-no-no-no-no-fifo-1core 20 40 482.sphinx3-1100B.champsimtrace.xz
./run_champsim.sh bimodal-no-no-no-no-lfu-1core 20 40 482.sphinx3-1100B.champsimtrace.xz
./run_champsim.sh bimodal-no-no-no-no-random-1core 20 40 482.sphinx3-1100B.champsimtrace.xz
