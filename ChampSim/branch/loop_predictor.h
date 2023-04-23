 #include <stdint.h>
#include <algorithm>


#define TABLE_ENTRIES 256
#define WAY 4
#define BITS_WAY 2
#define BITS_IND 6
#define TAG_LEN 14
#define MAXAGE 255
#define MAXCONF 5
struct Entry{
	uint32_t past_iter, curr_iter, tag, confidence, age;
	Entry(){
		past_iter = 0;
		curr_iter = 0;
		tag = 0;
		confidence = 0;
		age = 0;
	}
};

class LoopPredictor{
	Entry entries[256];

	public:
		bool pred;
		bool found;
		Entry *lastEntry;
		uint64_t ind;
		uint64_t tag;
		bool getPred(uint64_t ip){
			ind = (ip & ((1 << BITS_IND)-1)) << BITS_WAY;
			tag  = (ip>>BITS_IND & ((1<<TAG_LEN)-1));
			for(int i = 0; i < WAY; i++){
				if (entries[i+ind].tag == tag){
					/* hit = i; */
					found = true;
					lastEntry = &entries[i+ind];
					if(entries[i+ind].curr_iter + 1 == entries[i+ind].past_iter){
						pred = false;
						return false;
					}
					pred = true;
					return true;

				}
			}
			// Could not find the entry in the table
			found = false;
			pred = false;
			return false;
			
		}

		void update(bool taken, bool tage){
			if (found){
				if(taken != pred){
					*lastEntry = Entry();
					return;
				}
				//if (taken != tage){
				else{
                    if(lastEntry->age < MAXAGE)
					lastEntry->age++;
				}
				lastEntry->curr_iter++;
				if(lastEntry->curr_iter > lastEntry->past_iter){
					// This is the longest this loop has run, we have no idea
					lastEntry->past_iter = 0;
					lastEntry->age = 0;
					lastEntry->confidence = 0;
				}

				if (!taken){
					//The loop ends here
					if(lastEntry->curr_iter == lastEntry->past_iter){
						if (lastEntry->confidence < MAXCONF){
							lastEntry->confidence++;
						}
					}
					else{
						if(lastEntry->past_iter == 0){
							// We had reset because found a longer iter
							lastEntry->past_iter = lastEntry->curr_iter;
						}
						else{
							*lastEntry = Entry();
						}
					}
					lastEntry->curr_iter = 0; // Present loop has ended
				}

			}
			else if (taken){
				for (int i = 0; i < WAY; i++){
					int j = ind + i;
					if (entries[j].age <= 0){
						entries[j] = Entry();
						entries[j].tag = tag;
						entries[j].age = MAXAGE;
						entries[j].curr_iter = 1;
					}
					else{
						entries[j].age--;	
					}
				}
			}


		}
};
