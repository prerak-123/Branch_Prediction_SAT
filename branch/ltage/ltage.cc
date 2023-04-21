#include <cmath>
#include <stdlib.h>

#include "ooo_cpu.h"

const int PRED_BITS = 3;
const int USEFUL_BITS = 2;
const int NUM_COMPONENTS = 12;
const int BASE_PRED_SIZE = 4096;
const int GLOBAL_HISTORY_SIZE = 4096;
const int PERIODIC_UPDATE = 256000;
struct entry {
  int tag;

  // 0, 1, 2, 3 => Take
  //-4, -3, -2, -1 => Don't take
  int pred = 0;

  // 2 bit
  int u = 0;
};

class Component
{
public:
  std::vector<entry> entries;
  int hist_len;
  int tag_width;

  Component(int num_entries, int hist_len, int tag_width)
  {
    entries = std::vector<entry>(num_entries);
    this->hist_len = hist_len;
    this->tag_width = tag_width;
  }

  int get_index(uint64_t ip, std::vector<uint8_t>& history)
  {
    uint64_t index = ip >> 2;
    for (int i = 0; i < hist_len; i++) {
      int index_to_xor = i % 63;
      index = index ^ (((int64_t)history[i]) << index_to_xor);
    }

    return index % entries.size();
  }

  int get_tag(uint64_t ip, std::vector<uint8_t>& history)
  {
    int tag = ip;
    for (int i = 0; i < hist_len; i++) {
      int index_to_xor = i % tag_width;
      tag = tag ^ (((int)history[i]) << index_to_xor);
    }
    return tag % (1 << tag_width);
  }

  // 0, 1 if tag hit, else -1
  int predict(uint64_t ip, std::vector<uint8_t>& history)
  {
    int index = get_index(ip, history);
    int tag = get_tag(ip, history);

    if (tag == entries[index].tag) {
      return (entries[index].pred >> (PRED_BITS - 1));
    }

    else {
      return -1;
    }
  }
};

std::vector<Component> components;
int num_entries[NUM_COMPONENTS] = {1000, 1000, 2000, 2000, 2000, 2000, 1000, 1000, 1000, 1000, 500, 500};
int hist_lens[NUM_COMPONENTS] = {4, 6, 10, 16, 25, 40, 64, 101, 160, 254, 403, 640};
int tag_widths[NUM_COMPONENTS] = {7, 7, 8, 8, 9, 10, 11, 12, 12, 13, 14, 15};
int branches = 0;

// Is it global or specific to branch??
// Assuming branch specific for now
std::vector<std::vector<uint8_t>> global_hist(GLOBAL_HISTORY_SIZE);

// 2 bit counter: 0, 1 Taken
//  -2, -1 Not Taken
std::vector<int> base_pred(BASE_PRED_SIZE);

void O3_CPU::initialize_branch_predictor()
{
  // Initializing components
  for (int i = 0; i < NUM_COMPONENTS; i++) {
    components.push_back(Component(num_entries[i], hist_lens[i], tag_widths[i]));
  }

  // Initialising global history table
  for (int i = 0; i < GLOBAL_HISTORY_SIZE; i++) {
    for (int j = 0; j < hist_lens[NUM_COMPONENTS - 1]; j++) {
      global_hist[i].push_back(0);
    }
  }

  // Initialising base_predictor
  for (int i = 0; i < BASE_PRED_SIZE; i++) {
    base_pred[i] = 0;
  }
}

uint8_t O3_CPU::predict_branch(uint64_t ip, uint64_t predicted_target, uint8_t always_taken, uint8_t branch_type)
{
  // std::cerr << "Predict Started for IP " << ip << std::endl;
  auto history = global_hist[ip % GLOBAL_HISTORY_SIZE];
  int altpred = -1;
  int provider = -1;
  int prediction;

  for (int i = 0; i < NUM_COMPONENTS; i++) {
    int pred = components[i].predict(ip, history);
    if (pred != -1) {
      if (provider != -1) {
        altpred = provider;
        provider = i;
      } else {
        provider = i;
      }
    }
  }

  // std::cerr << "Found Provider" << std::endl;

  if (provider == -1) {
    prediction = (base_pred[ip % BASE_PRED_SIZE] >= 0);
    // std::cerr << "Exit using base Prediction" << std::endl;
    return (uint8_t)prediction;
  }

  int index = components[provider].get_index(ip, history);
  if (components[provider].entries[index].u == 0 && (components[provider].entries[index].pred == 0 || components[provider].entries[index].pred == -1)) {
    if (altpred == -1) {
      prediction = (base_pred[ip % BASE_PRED_SIZE] >= 0);
      // std::cerr << "Exit using alt base Prediction" << std::endl;
      return (uint8_t)prediction;
    }
    prediction = components[altpred].predict(ip, history);
    // std::cerr << "Exit using alt Prediction" << std::endl;
    return (uint8_t)prediction;
  }

  prediction = components[provider].predict(ip, history);
  // std::cerr << "Exit using provider prediction" << std::endl;
  return (uint8_t)prediction;
}

void O3_CPU::last_branch_result(uint64_t ip, uint64_t branch_target, uint8_t taken, uint8_t branch_type)
{
  // std::cerr << "Update Started for IP " << ip << std::endl;
  branches += 1;

  auto history = global_hist[ip % GLOBAL_HISTORY_SIZE];
  int altpred = -1;
  int provider = -1;
  int prediction;

  for (int i = 0; i < NUM_COMPONENTS; i++) {
    int pred = components[i].predict(ip, history);
    if (pred != -1) {
      if (provider != -1) {
        altpred = provider;
        provider = i;
      } else {
        provider = i;
      }
    }
  }

  // If base predictor was provider
  if (provider == -1) {
    prediction = (base_pred[ip % BASE_PRED_SIZE] >= 0);

    // If prediction incorrect, find new entry
    if (prediction != taken) {
      int start;
      int random = std::rand() % 4;
      if (random <= 1) {
        start = 0;
      } else if (random == 2) {
        start = 1;
      } else {
        start = 2;
      }

      for (int i = start; i < NUM_COMPONENTS; i++) {
        int index = components[i].get_index(ip, history);
        if (components[i].entries[index].u == 0) {
          components[i].entries[index].tag = components[i].get_tag(ip, history);
          components[i].entries[index].pred = 0;
          break;
        }
      }
    }

    // Update pred counter
    if (taken) {
      base_pred[ip % BASE_PRED_SIZE] = std::min(base_pred[ip % BASE_PRED_SIZE] + 1, 1);
    } else {
      base_pred[ip % BASE_PRED_SIZE] = std::max(base_pred[ip % BASE_PRED_SIZE] - 1, -2);
    }

    // std::cerr << "Exiting using base" << std::endl;
    return;
  }
  //Provider is one of the partially tagged counters
  int index = components[provider].get_index(ip, history);
  prediction = components[provider].predict(ip, history);
  int altprediction;

  if (altpred == -1) {
    altprediction = base_pred[ip % BASE_PRED_SIZE] >= 0;
  }

  else {
    altprediction = components[altpred].predict(ip, history);
  }

  // Update Useful Counter
  if (prediction != altprediction) {
    if (prediction == taken) {
      components[provider].entries[index].u = std::min(components[provider].entries[index].u + 1, (1 << USEFUL_BITS) - 1);
    }

    else {
      components[provider].entries[index].u = std::max(components[provider].entries[index].u - 1, 0);
    }
  }

  // Update prediction counter
  if (taken) {
    components[provider].entries[index].pred = std::min(components[provider].entries[index].pred + 1, (1 << (PRED_BITS - 1)) - 1);
  } else {
    components[provider].entries[index].pred = std::max(components[provider].entries[index].pred - 1, -1*(1 << (PRED_BITS - 1)));
  }

  // New entry on misprediction
  if (prediction != taken) {
    int start;
    int random = std::rand() % 4;
    if (random <= 1) {
      start = provider + 1;
    } else if (random == 2) {
      start = provider + 2;
    } else {
      start = provider + 3;
    }

    for (int i = start; i < NUM_COMPONENTS; i++) {
      int index = components[i].get_index(ip, history);
      if (components[i].entries[index].u == 0) {
        components[i].entries[index].tag = components[i].get_tag(ip, history);
        components[i].entries[index].pred = 0;
        break;
      }
    }
  }

  // Update Global History
  std::rotate(global_hist[ip % GLOBAL_HISTORY_SIZE].begin(), global_hist[ip % GLOBAL_HISTORY_SIZE].end() - 1, global_hist[ip % GLOBAL_HISTORY_SIZE].end());

  global_hist[ip % GLOBAL_HISTORY_SIZE][0] = taken;

  // Reset Useful bit
  //TODO

  // std::cerr << "Exit provider" << std::endl;

}
