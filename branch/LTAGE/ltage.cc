#include<stdlib.h>
#include<cmath>
#include "ooo_cpu.h"

const int PRED_BITS = 3;
const int USEFUL_BITS = 2;
const int NUM_COMPONENTS = 12;
const int BASE_PRED_SIZE = 4096;

struct entry {
  int tag;
  int pred = 0;
  int u = 0;
};

class Component{
  std::vector<entry> entries;
  int hist_len;
  int tag_width;

public:

  Component(int num_entries, int hist_len, int tag_width){
    entries = std::vector<entry>(num_entries);
    this->hist_len = hist_len;
    this->tag_width = tag_width;
  }

  int get_index(uint64_t ip, std::vector<uint8_t> &history){
    uint64_t index = ip;

    for(int i = 0; i < hist_len; i++){
      int index_to_xor = i % 63;
      index = index ^ (((int64_t)history[i]) << index_to_xor);
    }

    return index % entries.size();
  }

  int get_tag(uint64_t ip, std::vector<uint8_t> &history){
    int tag = ip;
    for(int i = 0; i < hist_len; i++){
      int index_to_xor = i % tag_width;
      tag = tag ^ (((int)history[i]) << index_to_xor);
    }
    return tag % (1 << tag_width);
  }

  // 0, 1 if tag hit, else -1
  int predict(uint64_t ip, std::vector<uint8_t> &history){
    int index = get_index(ip, history);
    int tag = get_tag(ip, history);

    if(tag == entries[index].tag){
      return (entries[index].pred >> (PRED_BITS - 1));
    }

    else{
      return -1;
    }

  }
};

std::vector<Component> components;
int num_entries[NUM_COMPONENTS] = {1000, 1000, 2000, 2000, 2000, 2000, 1000, 1000, 1000, 1000, 500, 500};
int hist_lens[NUM_COMPONENTS] = {4, 6, 10, 16, 25, 40, 64, 101, 160, 254, 403, 640};
int tag_widths[NUM_COMPONENTS] = {7, 8, 9, 10, 11, 12, 13, 14, 15};

//Is it global or specific to branch??
//Assuming global for now
std::vector<int8_t> global_hist;

std::vector<int> base_pred(BASE_PRED_SIZE);

// constexpr std::size_t BIMODAL_TABLE_SIZE = 16384;
// constexpr std::size_t BIMODAL_PRIME = 16381;
// constexpr std::size_t COUNTER_BITS = 2;

// std::map<O3_CPU*, std::array<int, BIMODAL_TABLE_SIZE>> bimodal_table;

void O3_CPU::initialize_branch_predictor()
{
  for(int i = 0; i < NUM_COMPONENTS; i++){
    components.push_back(Component(num_entries[i], hist_lens[i], tag_widths[i]));
  }

  for(int i = 0; i < hist_lens[NUM_COMPONENTS - 1]; i++){
    global_hist.push_back(0);
  }

  for(int i = 0; i < BASE_PRED_SIZE; i++ ){
    base_pred[i] = 0;
  }
}

uint8_t O3_CPU::predict_branch(uint64_t ip, uint64_t predicted_target, uint8_t always_taken, uint8_t branch_type)
{
  // uint32_t hash = ip % BIMODAL_PRIME;

  // return bimodal_table[this][hash] >= (1 << (COUNTER_BITS - 1));
}

void O3_CPU::last_branch_result(uint64_t ip, uint64_t branch_target, uint8_t taken, uint8_t branch_type)
{
  // uint32_t hash = ip % BIMODAL_PRIME;

  // if (taken)
  //   bimodal_table[this][hash] = std::min(bimodal_table[this][hash] + 1, ((1 << COUNTER_BITS) - 1));
  // else
  //   bimodal_table[this][hash] = std::max(bimodal_table[this][hash] - 1, 0);
}
