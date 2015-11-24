#ifndef SIM_H_
#define SIM_H_

struct cache
{
	long long size;
	long long b_size;
};

struct line_node
{
	int valid_bit;
	int search_index;
	long long tag_bits;
	struct line_node *next;
};

typedef struct cache CACHE;
typedef struct line_node LINE;

int directmap(unsigned long long hex, LINE list[], long long sbits, int bbits, long long tagbits, long long total_misses, long long total_hits, long long cold_misses);
int fullassoc(unsigned long long hex, LINE list[], long long bbits, long long num_lines, long long cold_misses,long long capacity_misses, long long total_misses, int q_index, long long tagbits, long long total_hits, int policy);
void enqueue(LINE *item, int q_i);
long long dequeue(int q_i);
void LRUhit(long long tagindex, int q_i);
void delete();
void setenq(LINE *item, int s_index, int q_i);
long long setdequeue(int q_i, int search_i);
void setLRUhit(long long tagindex, int q_i, int setindex);

#endif /* SIM_H_ */
