#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "sim.h"

	
	int memory_accesses=0;
	LINE *L1_q_head;
	LINE *L2_q_head;
	LINE *L3_q_head;
	LINE *SIM_q_head;
	
int main(int argc, char *argv[])
{	
	long long capacity_misses=0;
	long long conflict_misses=0;
	
	int argv_length=0;
	
	long long L1_total_hits=0;
	long long L2_total_hits=0;
	long long L3_total_hits=0;
	
	long long L1_cold_misses=0;
	long long L2_cold_misses=0;
	long long L3_cold_misses=0;
	
	long long L1_total_misses=0;
	long long L2_total_misses=0;
	long long L3_total_misses=0;
	
	for (int i=1; argv[i]!=NULL; i++)
	{
			argv_length++;
	}
	
	char help [3]= "-h";
	
	if (argv[1]!=NULL)
	{
		if (strcmp(argv[1], help)==0)
		{
			printf("\t Proper input parameters are:\n\n");
		
			printf("./cache-sim -l#size <L# size> -l#assoc <L# assoc> <block size> <replace alg> <trace file> \n\n");
			printf("1.# represents the number of the cache. It can be 1,2,3.\n");
			printf("2. L# size refers to the cache size.\n");
			printf("3. L# assoc refers to the associativity of the cache.\n");
			printf("4.<block size> is the size of the cache block, by power of 2.\n");
			printf("5.<replace alg> is either the FIFO or LRU replacement algorithm. \n");
			printf("6.<trace file> is the name of a file that contains memory access traces.\n");
			return 0;		
		}
	}
	
	if (argv_length<7)
	{
		fprintf(stderr, "Error: Refer to -h for proper command line input.\n");
		return 0;
	}
	
	if (argv_length>7 && argv_length<11)
	{
		fprintf(stderr, "Error: Refer to -h for proper command line input.\n");
		return 0;
	}
	if (argv_length>11 && argv_length<15)
	{
		fprintf(stderr, "Error: Refer to -h for proper command line input.\n");
		return 0;
	}
		
	char l1size[8]="-l1size";
	char l2size[8]="-l2size";
	char l3size[8]="-l3size";
	
	char l1assoc[9]="-l1assoc";
	char l2assoc[9]="-l2assoc";
	char l3assoc[9]="-l3assoc";

	char direct[7]="direct";
	char assoc[6]="assoc";
	char set_assoc[7]="assoc:";
	
	char fifo [5]= "fifo";
	char FIFO [5]= "FIFO";
	char lru [4]= "lru";
	char LRU [4]= "LRU";
	
	int policy_index= argv_length-1;

	long long one_size;
	long long two_size;
	long long three_size;
	
	long long block_size;
	
	long long L1_capacity_misses=0;
	long long L2_capacity_misses=0;
	long long L3_capacity_misses=0;
	
	int m_bits= 64;
	
	int block_index= argv_length-2;
	
	block_size= strtoll(argv[block_index], NULL,0);
	
	CACHE *temp=(CACHE*)malloc(sizeof(CACHE));

	if ((block_size!=0) && ((block_size & (block_size-1))) == 0)
	{	
		temp->b_size=block_size;
	}
		
	else
	{
		fprintf(stderr,"Error: Block Size Not a Power of Two\n");
		return 0;
	}
	
	int b_bits= log2(block_size);
	
	for (int i=0; argv[i]!=NULL; i++)
	{
		if (argv[1]!=NULL)
		{
			if (strcmp(argv[1],l1size)!=0)
			{
				fprintf(stderr, "Error: Invalid Input.\n");
				return 0;
			}
		
			else
			{
				if (argv[2]!=NULL)
				{
					one_size = strtoll(argv[2],NULL,0);
					
					if (one_size==0)
					{
						fprintf(stderr, "Error: Invalid L1 Cache Size. \n");
						return 0;
					}
				}
			}
		}
	}
	
	if (argv_length==11)
	{
		for (int i=0; argv[i]!=NULL; i++)
		{
			if (argv[5]!=NULL)
			{
				if (strcmp(argv[5],l2size)!=0)
				{
					fprintf(stderr, "Error: Invalid Input.\n");
					return 0;
				}
		
				else
				{
					if (argv[6]!=NULL)
					{
						two_size = strtoll(argv[6],NULL,0);
					
						if (two_size==0)
						{
							fprintf(stderr, "Error: Invalid L2 Cache Size. \n");
							return 0;
						}
					}
				}
			}
		}
	}
	
	if (argv_length==15)
	{
		for (int i=0; argv[i]!=NULL; i++)
		{
			if (argv[5]!=NULL)
			{
				if (strcmp(argv[5],l2size)!=0)
				{
					fprintf(stderr, "Error: Invalid Input.\n");
					return 0;
				}
		
				else
				{
					if (argv[6]!=NULL)
					{
						two_size = strtoll(argv[6],NULL,0);
					
						if (two_size==0)
						{
							fprintf(stderr, "Error: Invalid L2 Cache Size. \n");
							return 0;
						}
					}
				}
			}
			
			if (argv[9]!=NULL)
			{
				if (strcmp(argv[9],l3size)!=0)
				{
					fprintf(stderr, "Error: Invalid Input.\n");
					return 0;
				}
		
				else
				{
					if (argv[10]!=NULL)
					{
						three_size = strtoll(argv[10],NULL,0);
					
						if (three_size==0)
						{
							fprintf(stderr, "Error: Invalid L3 Cache Size. \n");
							return 0;
						}
					}
				}
			}
		}
	}
	
	if (strcmp(argv[policy_index], fifo) !=0 && strcmp( argv[policy_index], FIFO) !=0 && strcmp(argv[policy_index],lru) !=0 && strcmp(argv[policy_index],LRU) !=0)
	{
		fprintf(stderr, "Error: Invalid Input.\n");
		return 0;
	}
	
	int policy_length=strlen(argv[policy_index]);
	int policy_flag=0;
	
	if (policy_length==4) /*FIFO queue selection*/
	{
		policy_flag=1;
	}
	
	if (policy_length==3) /*LRU selection */
	{
		policy_flag=2;
	}
	
	long long L1_s_bits=0;
	long long L2_s_bits=0;
	long long L3_s_bits=0;
	
	long long L1_tag_bits=0;
	long long L2_tag_bits=0;
	long long L3_tag_bits=0;
	
	long long L1_sets=0;
	long long L2_sets=0;
	long long L3_sets=0;
	
	long long L1_numlines=0;
	long long L2_numlines=0;
	long long L3_numlines=0;
	
	if (argv_length==15)
	{
		if (strcmp(argv[4],direct)==0)
		{
			L1_numlines=1;
		
			L1_sets= one_size/block_size;
		
			L1_s_bits= log2(L1_sets);
		
			L1_tag_bits= m_bits-(L1_s_bits+b_bits);
		}
			
		if (strcmp(argv[8],direct)==0)
		{
			L2_numlines=1;
		
			L2_sets= two_size/block_size;
		
			L2_s_bits= log2(L2_sets);
		
			L2_tag_bits= m_bits-(L2_s_bits+b_bits);
		}
		
		if (strcmp(argv[12],direct)==0)
		{
			L3_numlines=1;
		
			L3_sets= three_size/block_size;
		
			L3_s_bits= log2(L3_sets);
		
			L3_tag_bits= m_bits-(L3_s_bits+b_bits);
		}
		
		if (strcmp(argv[4], assoc) ==0)
		{
			L1_sets=1;
		
			L1_s_bits= 0;
		
			L1_numlines= one_size/block_size;
		
			L1_tag_bits= m_bits-(L1_s_bits+b_bits);
		}
		
		if (strcmp(argv[8], assoc) ==0)
		{
			L2_sets=1;
		
			L2_s_bits= 0;
		
			L2_numlines= two_size/block_size;
		
			L2_tag_bits= m_bits-(L2_s_bits+b_bits);
		}
		
		if (strcmp(argv[12], assoc) ==0)
		{
			L3_sets=1;
		
			L3_s_bits= 0;
		
			L3_numlines= three_size/block_size;
		
			L3_tag_bits= m_bits-(L3_s_bits+b_bits);
		}				
	}
	
	if(argv_length==11)
	{
		if (strcmp(argv[4],direct)==0)
		{
			L1_numlines=1;
		
			L1_sets= one_size/block_size;
		
			L1_s_bits= log2(L1_sets);
		
			L1_tag_bits= m_bits-(L1_s_bits+b_bits);
		}
			
		if (strcmp(argv[8],direct)==0)
		{
			L2_numlines=1;
		
			L2_sets= two_size/block_size;
		
			L2_s_bits= log2(L2_sets);
		
			L2_tag_bits= m_bits-(L2_s_bits+b_bits);
		}
		if (strcmp(argv[4], assoc) ==0)
		{
			L1_sets=1;
		
			L1_s_bits= 0;
		
			L1_numlines= one_size/block_size;
		
			L1_tag_bits= m_bits-(L1_s_bits+b_bits);
		}
		
		if (strcmp(argv[8], assoc) ==0)
		{
			L2_sets=1;
		
			L2_s_bits= 0;
		
			L2_numlines= two_size/block_size;
		
			L2_tag_bits= m_bits-(L2_s_bits+b_bits);
		}
			
	}
	
	if(argv_length==7)
	{
		if (strcmp(argv[4],direct)==0)
		{
			L1_numlines=1;
		
			L1_sets= one_size/block_size;
		
			L1_s_bits= log2(L1_sets);
		
			L1_tag_bits= m_bits-(L1_s_bits+b_bits);
		}
	
		if (strcmp(argv[4], assoc) ==0)
		{
			L1_sets=1;
		
			L1_s_bits= 0;
		
			L1_numlines= one_size/block_size;
		
			L1_tag_bits= m_bits-(L1_s_bits+b_bits);
		}
	}
	
	int leng=0;
	int leng2=0;
	int leng3=0;
	
	long long L1_nway_value=0;
	long long L2_nway_value=0;
	long long L3_nway_value=0;
	
	long long L1_sets_test=0;
	long long L2_sets_test=0;
	long long L3_sets_test=0;
	
	if (argv_length==7)
	{
		leng=strlen(argv[4]);
		
		if (leng>5 && (strcmp(argv[4],direct)!=0))
		{		
			if(strstr(argv[4], set_assoc)==NULL)
			{
				fprintf(stderr,"Error: Invalid Input.\n");
			}
		
			else
			{
				char *pch= strtok(argv[4], set_assoc);
				L1_nway_value= strtol(pch,NULL,0);
			
				if (L1_nway_value==0)
				{
					fprintf(stderr, "Error: Invalid n-way output.\n");
					return 0;
				}
			
				L1_numlines=L1_nway_value;
				L1_sets_test= (one_size)/(block_size*L1_nway_value);
			
				if((L1_sets_test!=0) && ((L1_sets_test & (L1_sets_test-1))) != 0)
				{
					fprintf(stderr,"Error: Invalid N-way value.\n");
					return 0;
				}
			
				L1_s_bits=log2(L1_sets_test);
				L1_tag_bits= m_bits-(L1_s_bits+b_bits);
			
			}
		}
	}
	
	if (argv_length==11)
	{
		leng=strlen(argv[4]);
		leng2=strlen(argv[8]);
		
		if (leng>5 && (strcmp(argv[4],direct)!=0))
		{		
			if(strstr(argv[4], set_assoc)==NULL)
			{
				fprintf(stderr,"Error: Invalid Input.\n");
			}
		
			else
			{
				char *pch= strtok(argv[4], set_assoc);
				L1_nway_value= strtol(pch,NULL,0);
			
				if (L1_nway_value==0)
				{
					fprintf(stderr, "Error: Invalid n-way output.\n");
					return 0;
				}
			
				L1_numlines=L1_nway_value;
				L1_sets_test= (one_size)/(block_size*L1_nway_value);
			
				if((L1_sets_test!=0) && ((L1_sets_test & (L1_sets_test-1))) != 0)
				{
					fprintf(stderr,"Error: Invalid N-way value.\n");
					return 0;
				}
			
				L1_s_bits=log2(L1_sets_test);
				L1_tag_bits= m_bits-(L1_s_bits+b_bits);
			
			}
		}
		
		if (leng2>5 && (strcmp(argv[8],direct)!=0))
		{		
			if(strstr(argv[8], set_assoc)==NULL)
			{
				fprintf(stderr,"Error: Invalid Input.\n");
			}
		
			else
			{
				char *pch= strtok(argv[8], set_assoc);
				L2_nway_value= strtol(pch,NULL,0);
			
				if (L2_nway_value==0)
				{
					fprintf(stderr, "Error: Invalid n-way output.\n");
					return 0;
				}
			
				L2_numlines=L2_nway_value;
				L2_sets_test= (two_size)/(block_size*L2_nway_value);
			
				if((L2_sets_test!=0) && ((L2_sets_test & (L2_sets_test-1))) != 0)
				{
					fprintf(stderr,"Error: Invalid N-way value.\n");
					return 0;
				}
			
				L2_s_bits=log2(L2_sets_test);
				L2_tag_bits= m_bits-(L2_s_bits+b_bits);
			
			}
		}
	}
	
	if (argv_length==15)
	{
		leng=strlen(argv[4]);
		leng2=strlen(argv[8]);
		leng3=strlen(argv[12]);
		
		if (leng>5 && (strcmp(argv[4],direct)!=0))
		{		
			if(strstr(argv[4], set_assoc)==NULL)
			{
				fprintf(stderr,"Error: Invalid Input.\n");
			}
		
			else
			{
				char *pch= strtok(argv[4], set_assoc);
				L1_nway_value= strtol(pch,NULL,0);
			
				if (L1_nway_value==0)
				{
					fprintf(stderr, "Error: Invalid n-way output.\n");
					return 0;
				}
			
				L1_numlines=L1_nway_value;
				L1_sets_test= (one_size)/(block_size*L1_nway_value);
			
				if((L1_sets_test!=0) && ((L1_sets_test & (L1_sets_test-1))) != 0)
				{
					fprintf(stderr,"Error: Invalid N-way value.\n");
					return 0;
				}
			
				L1_s_bits=log2(L1_sets_test);
				L1_tag_bits= m_bits-(L1_s_bits+b_bits);
			
			}
		}
		
		if (leng2>5 && (strcmp(argv[8],direct)!=0))
		{		
			if(strstr(argv[8], set_assoc)==NULL)
			{
				fprintf(stderr,"Error: Invalid Input.\n");
			}
		
			else
			{
				char *pch= strtok(argv[8], set_assoc);
				L2_nway_value= strtol(pch,NULL,0);
			
				if (L2_nway_value==0)
				{
					fprintf(stderr, "Error: Invalid n-way output.\n");
					return 0;
				}
			
				L2_numlines=L2_nway_value;
				L2_sets_test= (two_size)/(block_size*L2_nway_value);
			
				if((L2_sets_test!=0) && ((L2_sets_test & (L2_sets_test-1))) != 0)
				{
					fprintf(stderr,"Error: Invalid N-way value.\n");
					return 0;
				}
			
				L2_s_bits=log2(L2_sets_test);
				L2_tag_bits= m_bits-(L2_s_bits+b_bits);
			
			}
		}
		
		if (leng3>5 && (strcmp(argv[12],direct)!=0))
		{		
			if(strstr(argv[12], set_assoc)==NULL)
			{
				fprintf(stderr,"Error: Invalid Input.\n");
			}
		
			else
			{
				char *pch= strtok(argv[12], set_assoc);
				L3_nway_value= strtol(pch,NULL,0);
			
				if (L3_nway_value==0)
				{
					fprintf(stderr, "Error: Invalid n-way output.\n");
					return 0;
				}
			
				L3_numlines=L3_nway_value;
				L3_sets_test= (three_size)/(block_size*L3_nway_value);
			
				if((L3_sets_test!=0) && ((L3_sets_test & (L3_sets_test-1))) != 0)
				{
					fprintf(stderr,"Error: Invalid N-way value.\n");
					return 0;
				}
			
				L3_s_bits=log2(L3_sets_test);
				L3_tag_bits= m_bits-(L3_s_bits+b_bits);
			
			}
		}
	}
	
	FILE *fp= fopen(argv[argv_length], "r");
	
	if (fp==NULL)
	{
		fprintf(stderr, "Inccorect Trace File Name.\n");
		return 0;
	}
	
	unsigned long long hex_value=0;
	
	char hex_array[20];
		
	LINE L1_set_list[L1_sets]; /* for direct-mapped use */
	LINE L2_set_list[L2_sets];
	LINE L3_set_list[L3_sets];
	
	if (strcmp(argv[4],direct)==0)
	{
		for (int j=0; j<L1_sets; j++)
		{
			L1_set_list[j].valid_bit=0;
			L1_set_list[j].tag_bits=0;
		}
	}
	
	if(argv_length>7)
	{
		if (strcmp(argv[8],direct)==0)
		{
			for (int j=0; j<L2_sets; j++)
			{
				L2_set_list[j].valid_bit=0;
				L2_set_list[j].tag_bits=0;
			}	
		}
	}
	
	if(argv_length>11)
	{
		if (strcmp(argv[12],direct)==0)
		{
			for (int j=0; j<L3_sets; j++)
			{
				L3_set_list[j].valid_bit=0;
				L3_set_list[j].tag_bits=0;
			}
		}
	}
		
	LINE L1_line_list[L1_numlines]; /* for fully-associative use */
	LINE L2_line_list[L2_numlines];
	LINE L3_line_list[L3_numlines];
	
	if (strcmp(argv[4],assoc)==0)
	{
		for (int i=0; i<L1_numlines; i++)
		{
			L1_line_list[i].valid_bit=0;
			L1_line_list[i].tag_bits=0;
		}
	}
	
	if(argv_length>7)
	{
		if (strcmp(argv[8],assoc)==0)
		{
			for (int i=0; i<L2_numlines; i++)
			{
				L2_line_list[i].valid_bit=0;
				L2_line_list[i].tag_bits=0;
			}
		}
	}
	
	if(argv_length>11)
	{
		if (strcmp(argv[12],assoc)==0)
		{
			for (int i=0; i<L3_numlines; i++)
			{
				L3_line_list[i].valid_bit=0;
				L3_line_list[i].tag_bits=0;
			}
		}
	}
	
	
	LINE L1_set_ass[L1_sets_test][L1_numlines]; /*set associative*/
	LINE L2_set_ass[L2_sets_test][L2_numlines];
	LINE L3_set_ass[L3_sets_test][L3_numlines];
	
	if(leng>5 && (strcmp(argv[4],direct)!=0))
	{
		for (int i=0; i<L1_sets_test; i++)
		{
			for (int j=0; j<L1_numlines; j++)
			{
				L1_set_ass[i][j].valid_bit=0;
				L1_set_ass[i][j].tag_bits=0;
			}
		}
	}
	if(leng2>5 && (strcmp(argv[8],direct)!=0))
	{
		for (int i=0; i<L2_sets_test; i++)
		{
			for (int j=0; j<L2_numlines; j++)
			{
				L2_set_ass[i][j].valid_bit=0;
				L2_set_ass[i][j].tag_bits=0;
			}
		}
	}
	if(leng3>5 && (strcmp(argv[12],direct)!=0))
	{
		for (int i=0; i<L3_sets_test; i++)
		{
			for (int j=0; j<L3_numlines; j++)
			{
				L3_set_ass[i][j].valid_bit=0;
				L3_set_ass[i][j].tag_bits=0;
			}
		}
	}
	
	long long setassoc_mask= L1_sets_test-1;
	int setassoc_i=0;
	LINE *holder;
	long long counter=L1_numlines;
	long long num_elements=0;
	int searchi=0;
	
	long long remaining_space=0;
	long long fullsize=(one_size)/(block_size);
	int cold_flag=0;
	long long tag_index=0;
	
	long long tag_sumsize= pow(2,L1_tag_bits);
	long long tag_mask= tag_sumsize-1;
	
	long long temp_hold=0;
	long long temp_cold_hold=0;
	
	long long L1_sim_misses=0;
	
	
	long long L1_sim_cold=0;
	
	
	long long L1_sim_capacity=0;
	
	
	long long L1_sim_size=(one_size)/(block_size);
	
	LINE L1_sim[L1_sim_size];
	
	long long L1_sim_tagbits=0;
	
	
	long long L1_sim_hits=0;
	
	
	long long temp_hold_hit=0;
	long long tagbitshift=0;
	
	int function_result=0;
	
	for (int i=0; i<L1_sim_size; i++)
	{
		L1_sim[i].valid_bit=0;
		L1_sim[i].tag_bits=0;
	}
	
	while (fgets(hex_array, 100, fp) !=NULL)
	{
		memory_accesses++;
		
		hex_value= strtoull(hex_array, NULL,16);
		
		if(argv_length==7) /* 1 cache simulation */
		{
			if (strcmp(argv[4],direct)==0)
			{
				temp_hold=L1_total_misses;
				temp_hold_hit=L1_total_hits;
				temp_cold_hold=L1_cold_misses;
				
				function_result=directmap(hex_value, L1_set_list, L1_s_bits, b_bits, L1_tag_bits, L1_total_misses, L1_total_hits, L1_cold_misses);
				
				if(function_result==1)
				{
					L1_cold_misses++;
				}
				
				if(function_result==3)
				{
					L1_total_hits++;
				}
				else
				{
					L1_total_misses++;
				}
				
				 /* must simulate full associative cache.*/
				
				L1_sim_tagbits= m_bits-(b_bits);
					
				function_result=fullassoc(hex_value, L1_sim, b_bits, L1_sim_size, L1_sim_cold, L1_sim_capacity, L1_sim_misses, 4,L1_sim_tagbits,L1_sim_hits, 2);
					
				if(function_result==1)
				{
					L1_sim_cold++;
				}
				
				if(function_result==3)
				{
					L1_sim_hits++;
				}
				else
				{
					L1_sim_misses++;
				}
			}
			else if (strcmp(argv[4],assoc)==0)
			{	
				temp_hold=L1_total_misses;
				temp_cold_hold=L1_cold_misses;
				
				function_result=fullassoc(hex_value, L1_line_list, b_bits, L1_numlines, L1_cold_misses,L1_capacity_misses, L1_total_misses,1,L1_tag_bits,L1_total_hits,policy_flag);
				
				if(function_result==1)
				{
					L1_cold_misses++;
					
				}
				
				if(function_result==3)
				{
					L1_total_hits++;
				}
				else
				{
					L1_total_misses++;
				}
			}
			else if (leng>5 && (strcmp(argv[4],direct)!=0))
			{
				temp_hold_hit=L1_total_hits;
				
				temp_hold=L1_total_misses;
				
				temp_cold_hold=L1_cold_misses;
				
				counter= L1_numlines;
				
				searchi=0;
				
				num_elements=0;
				
				fullsize=(one_size)/(block_size);
				
				cold_flag=0;
				
				setassoc_mask= L1_sets_test-1;
				
				setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
				tagbitshift= L1_s_bits+b_bits;
				
				tag_sumsize=pow(2,L1_tag_bits);
				
				tag_mask=tag_sumsize-1;
				
				tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
				holder= &L1_set_ass[setassoc_i][0];
				
				for (int i=0; i<L1_numlines;i++)
				{
					if (L1_set_ass[setassoc_i][i].valid_bit==1)
					{
						num_elements++;
					}
				}
						
				remaining_space=L1_numlines-num_elements;
				
				while (counter>0)
				{
					if (holder->tag_bits==tag_index)
					{
						cold_flag=1;
						break;
					}
					
					else
					{
						searchi++;
						
						if (&L1_set_ass[setassoc_i][searchi]!=NULL)
						{
							holder=&L1_set_ass[setassoc_i][searchi];
						}
					}
					
					counter--;
				}
				
				if (cold_flag==1)
				{
					L1_total_hits++;
					
					if (policy_flag==2)
					{
						setLRUhit(tag_index, 1,setassoc_i);
					}
				}
				
				else if (cold_flag==0)
				{
					L1_total_misses++;
								
					if(remaining_space!=0)
					{
						searchi=0;
						holder=&L1_set_ass[setassoc_i][0];
								
						while (holder->valid_bit!=0)
						{
							searchi++;
					
							if (&L1_set_ass[setassoc_i][searchi]!=NULL)
							{
								holder=&L1_set_ass[setassoc_i][searchi];
							}
						}
										
						holder->valid_bit=1;
						holder->tag_bits= tag_index;
						L1_cold_misses++;
						setenq(holder,setassoc_i,1); 
					}
							
					else if(remaining_space==0)
					{
						long long tag_replace=0;
										
						tag_replace=setdequeue(1,setassoc_i);
										
						LINE *guide=&L1_set_ass[setassoc_i][0];
						int a_index=0;
										
						while (guide->tag_bits!=tag_replace)
						{
							a_index++;
											
							if (&L1_set_ass[setassoc_i][a_index]!=NULL)
							{
								guide=&L1_set_ass[setassoc_i][a_index];
							}
						}	
							
						guide->tag_bits=tag_index;
										
						setenq(guide, setassoc_i,1);
					}
									
				}
				/* simulate full associative */
				
				L1_sim_tagbits= m_bits-(b_bits);
					
				function_result=fullassoc(hex_value, L1_sim, b_bits, L1_sim_size, L1_sim_cold, L1_sim_capacity, L1_sim_misses, 4,L1_sim_tagbits,L1_sim_hits, 2);
					
				if(function_result==1)
				{
					L1_sim_cold++;
				}
				
				if(function_result==3)
				{
					L1_sim_hits++;
				}
				else
				{
					L1_sim_misses++;
				}
			}
		}
		if(argv_length==11) /* 2 cache simulation */
		{
			if (strcmp(argv[4],direct)==0)
			{
				temp_hold=L1_total_misses;
				temp_hold_hit=L1_total_hits;
				temp_cold_hold=L1_cold_misses;
				
				function_result=directmap(hex_value, L1_set_list, L1_s_bits, b_bits, L1_tag_bits, L1_total_misses, L1_total_hits, L1_cold_misses);
				
				if(function_result==1)
				{
					L1_cold_misses++;
				}
				
				if(function_result==3)
				{
					L1_total_hits++;
				}
				else
				{
					L1_total_misses++;
				}
				
				 /* must simulate full associative cache.*/
				
					L1_sim_tagbits= m_bits-(b_bits);
					
					function_result=fullassoc(hex_value, L1_sim, b_bits, L1_sim_size, L1_sim_cold, L1_sim_capacity, L1_sim_misses, 4,L1_sim_tagbits,L1_sim_hits, 2);
					
					if(function_result==1)
					{
						L1_sim_cold++;
					}
				
					if(function_result==3)
					{
						L1_sim_hits++;
					}
					else
					{
						L1_sim_misses++;
					}
					
				if (L1_total_misses>temp_hold) /* go to 2nd cache */ 
				{ 
					if (strcmp(argv[8],direct)==0)
					{
						function_result= directmap(hex_value, L2_set_list, L2_s_bits, b_bits, L2_tag_bits, L2_total_misses, L2_total_hits, L2_cold_misses);
						
						if(function_result==1)
						{
							L2_cold_misses++;
						}
				
						if(function_result==3)
						{
							L2_total_hits++;
						}
						else
						{
							L2_total_misses++;
						}
					}
					else if (strcmp(argv[8],assoc)==0) /* full associative*/
					{
						function_result=fullassoc(hex_value, L2_line_list, b_bits, L2_numlines, L2_cold_misses,L2_capacity_misses, L2_total_misses, 2, L2_tag_bits, L2_total_hits,policy_flag);
							
						if(function_result==1)
						{
							L2_cold_misses++;
						}
				
						if(function_result==3)
						{
							L2_total_hits++;
						}
						
						else
						{
							L2_total_misses++;
						}
					}
					else if (leng2>5 && (strcmp(argv[8],direct)!=0)) /* set associative */
					{
						counter= L2_numlines;
				
						searchi=0;
				
						num_elements=0;
				
						fullsize=(two_size)/(block_size);
				
						cold_flag=0;
				
						setassoc_mask= L2_sets_test-1;
				
						setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
						tagbitshift= L2_s_bits+b_bits;
				
						tag_sumsize=pow(2,L2_tag_bits);
				
						tag_mask=tag_sumsize-1;
				
						tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
						holder= &L2_set_ass[setassoc_i][0];
				
						for (int i=0; i<L2_numlines;i++)
						{
							if (L2_set_ass[setassoc_i][i].valid_bit==1)
							{
								num_elements++;
							}
						}
						
						remaining_space=L2_numlines-num_elements;
				
						while (counter>0)
						{
							if (holder->tag_bits==tag_index)
							{
								cold_flag=1;
								break;
							}
					
							else
							{
								searchi++;
						
								if (&L2_set_ass[setassoc_i][searchi]!=NULL)
								{
									holder=&L2_set_ass[setassoc_i][searchi];
								}
							}
					
							counter--;
						}
				
						if (cold_flag==1)
						{
							L2_total_hits++;
					
							if (policy_flag==2)
							{
								setLRUhit(tag_index, 2,setassoc_i);
							}
						}
				
						else if (cold_flag==0)
						{
							L2_total_misses++;
									
							if(remaining_space!=0)
							{
								searchi=0;
								holder=&L2_set_ass[setassoc_i][0];
								
								while (holder->valid_bit!=0)
								{
									searchi++;
					
									if (&L2_set_ass[setassoc_i][searchi]!=NULL)
									{
										holder=&L2_set_ass[setassoc_i][searchi];
									}
								}
										
								holder->valid_bit=1;
								holder->tag_bits= tag_index;
								L2_cold_misses++;
								setenq(holder,setassoc_i,2); 
							}
							
							else if(remaining_space==0)
							{
								long long tag_replace=0;
										
								tag_replace=setdequeue(2,setassoc_i);
										
								LINE *guide=&L2_set_ass[setassoc_i][0];
								int a_index=0;
										
								while (guide->tag_bits!=tag_replace)
								{
									a_index++;
											
									if (&L2_set_ass[setassoc_i][a_index]!=NULL)
									{
										guide=&L2_set_ass[setassoc_i][a_index];
									}
								}
										
								guide->tag_bits=tag_index;
										
								setenq(guide, setassoc_i,2);
							}
						}
					}
				}
			}
			else if (strcmp(argv[4],assoc)==0)
			{	
				temp_hold=L1_total_misses;
				temp_cold_hold=L1_cold_misses;
				
				function_result=fullassoc(hex_value, L1_line_list, b_bits, L1_numlines, L1_cold_misses,L1_capacity_misses, L1_total_misses,1,L1_tag_bits,L1_total_hits,policy_flag);
				
				if(function_result==1)
				{
					L1_cold_misses++;
					
				}
				
				if(function_result==3)
				{
					L1_total_hits++;
				}
				else
				{
					L1_total_misses++;
				}
				
				if (L1_total_misses>temp_hold) /* go to 2nd cache */ 
				{
					if (strcmp(argv[8],direct)==0)
					{
						function_result=directmap(hex_value, L2_set_list, L2_s_bits, b_bits, L2_tag_bits, L2_total_misses, L2_total_hits, L2_cold_misses);
						
						if(function_result==1)
						{
							L2_cold_misses++;
							
						}
				
						if(function_result==3)
						{
							L2_total_hits++;
						}
						
						else
						{
							L2_total_misses++;
						}
					}
					else if (strcmp(argv[8],assoc)==0) /* full associative*/
					{
						function_result=fullassoc(hex_value, L2_line_list, b_bits, L2_numlines, L2_cold_misses,L2_capacity_misses, L2_total_misses, 2, L2_tag_bits, L2_total_hits,policy_flag);
						
						if(function_result==1)
						{
							L2_cold_misses++;
							
						}
				
						if(function_result==3)
						{
							L2_total_hits++;
						}
						
						else
						{
							L2_total_misses++;
						}
					}
					else if (leng2>5 && (strcmp(argv[8],direct)!=0)) /* set associative */
					{
						temp_hold_hit=L2_total_hits;
				
						temp_hold=L2_total_misses;
				
						temp_cold_hold=L2_cold_misses;
				
						counter= L2_numlines;
				
						searchi=0;
				
						num_elements=0;
				
						fullsize=(two_size)/(block_size);
				
						cold_flag=0;
				
						setassoc_mask= L2_sets_test-1;
				
						setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
						tagbitshift= L2_s_bits+b_bits;
				
						tag_sumsize=pow(2,L2_tag_bits);
				
						tag_mask=tag_sumsize-1;
				
						tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
						holder= &L2_set_ass[setassoc_i][0];
				
							for (int i=0; i<L2_numlines;i++)
						{
							if (L2_set_ass[setassoc_i][i].valid_bit==1)
							{
								num_elements++;
							}
						}
						
						remaining_space=L2_numlines-num_elements;
				
						while (counter>0)
						{
							if (holder->tag_bits==tag_index)
							{
								cold_flag=1;
								break;
							}
					
							else
							{
								searchi++;
						
								if (&L2_set_ass[setassoc_i][searchi]!=NULL)
								{
									holder=&L2_set_ass[setassoc_i][searchi];
								}
							}
					
							counter--;
						}
				
						if (cold_flag==1)
						{
							L2_total_hits++;
					
							if (policy_flag==2)
							{
								setLRUhit(tag_index, 2,setassoc_i);
							}
						}
				
						else if (cold_flag==0)
						{
							L2_total_misses++;
									
							if(remaining_space!=0)
							{
								searchi=0;
								holder=&L2_set_ass[setassoc_i][0];
								
								while (holder->valid_bit!=0)
								{
									searchi++;
					
									if (&L2_set_ass[setassoc_i][searchi]!=NULL)
									{
										holder=&L2_set_ass[setassoc_i][searchi];
									}
								}
										
								holder->valid_bit=1;
								holder->tag_bits= tag_index;
								L2_cold_misses++;
								setenq(holder,setassoc_i,2); 
							}
							
							else if(remaining_space==0)
							{
								long long tag_replace=0;
										
								tag_replace=setdequeue(2,setassoc_i);
										
								LINE *guide=&L2_set_ass[setassoc_i][0];
								int a_index=0;
										
								while (guide->tag_bits!=tag_replace)
								{
									a_index++;
											
									if (&L2_set_ass[setassoc_i][a_index]!=NULL)
									{
										guide=&L2_set_ass[setassoc_i][a_index];
									}
								}
										
								guide->tag_bits=tag_index;
										
								setenq(guide, setassoc_i,2);
							}
						}
					}
				}
			}
			else if (leng>5 && (strcmp(argv[4],direct)!=0))
			{
				temp_hold_hit=L1_total_hits;
				
				temp_hold=L1_total_misses;
				
				temp_cold_hold=L1_cold_misses;
				
				counter= L1_numlines;
				
				searchi=0;
				
				num_elements=0;
				
				fullsize=(one_size)/(block_size);
				
				cold_flag=0;
				
				setassoc_mask= L1_sets_test-1;
				
				setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
				tagbitshift= L1_s_bits+b_bits;
				
				tag_sumsize=pow(2,L1_tag_bits);
				
				tag_mask=tag_sumsize-1;
				
				tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
				holder= &L1_set_ass[setassoc_i][0];
				
				for (int i=0; i<L1_numlines;i++)
				{
					if (L1_set_ass[setassoc_i][i].valid_bit==1)
					{
						num_elements++;
					}
				}
						
				remaining_space=L1_numlines-num_elements;
				
				while (counter>0)
				{
					if (holder->tag_bits==tag_index)
					{
						cold_flag=1;
						break;
					}
					
					else
					{
						searchi++;
						
						if (&L1_set_ass[setassoc_i][searchi]!=NULL)
						{
							holder=&L1_set_ass[setassoc_i][searchi];
						}
					}
					
					counter--;
				}
				
				if (cold_flag==1)
				{
					L1_total_hits++;
					
					if (policy_flag==2)
					{
						setLRUhit(tag_index, 1,setassoc_i);
					}
				}
				
				else if (cold_flag==0)
				{
					L1_total_misses++;
								
					if(remaining_space!=0)
					{
						searchi=0;
						holder=&L1_set_ass[setassoc_i][0];
								
						while (holder->valid_bit!=0)
						{
							searchi++;
					
							if (&L1_set_ass[setassoc_i][searchi]!=NULL)
							{
								holder=&L1_set_ass[setassoc_i][searchi];
							}
						}
										
						holder->valid_bit=1;
						holder->tag_bits= tag_index;
						L1_cold_misses++;
						setenq(holder,setassoc_i,1); 
					}
							
					else if(remaining_space==0)
					{
						long long tag_replace=0;
										
						tag_replace=setdequeue(1,setassoc_i);
										
						LINE *guide=&L1_set_ass[setassoc_i][0];
						int a_index=0;
										
						while (guide->tag_bits!=tag_replace)
						{
							a_index++;
											
							if (&L1_set_ass[setassoc_i][a_index]!=NULL)
							{
								guide=&L1_set_ass[setassoc_i][a_index];
							}
						}	
							
						guide->tag_bits=tag_index;
										
						setenq(guide, setassoc_i,1);
					}
									
				}
				/* simulate full associative */
				
					L1_sim_tagbits= m_bits-(b_bits);
					
					function_result=fullassoc(hex_value, L1_sim, b_bits, L1_sim_size, L1_sim_cold, L1_sim_capacity, L1_sim_misses, 4,L1_sim_tagbits,L1_sim_hits, 2);
					
					if(function_result==1)
					{
						L1_sim_cold++;
					}
				
					if(function_result==3)
					{
						L1_sim_hits++;
					}
					else
					{
						L1_sim_misses++;
					}
				
				if (L1_total_misses>temp_hold) /* go to second cache */
				{
					if (strcmp(argv[8],direct)==0)
					{
						function_result= directmap(hex_value, L2_set_list, L2_s_bits, b_bits, L2_tag_bits, L2_total_misses, L2_total_hits, L2_cold_misses);
						
						if(function_result==1)
						{
							L2_cold_misses++;
						}
				
						if(function_result==3)
						{
							L2_total_hits++;
						}
						else
						{
							L2_total_misses++;
						}
					}
					else if (strcmp(argv[8],assoc)==0) /* full associative*/
					{
						function_result=fullassoc(hex_value, L2_line_list, b_bits, L2_numlines, L2_cold_misses,L2_capacity_misses, L2_total_misses, 2, L2_tag_bits, L2_total_hits,policy_flag);
						
						if(function_result==1)
						{
							L2_cold_misses++;
						}
				
						if(function_result==3)
						{
							L2_total_hits++;
						}
						
						else
						{
							L2_total_misses++;
						}
					}
					else if (leng2>5 && (strcmp(argv[8],direct)!=0)) /* set associative */
					{
						temp_hold_hit=L2_total_hits;
				
						temp_hold=L2_total_misses;
				
						temp_cold_hold=L2_cold_misses;
				
						counter= L2_numlines;
				
						searchi=0;
				
						num_elements=0;
				
						fullsize=(two_size)/(block_size);
				
						cold_flag=0;
				
						setassoc_mask= L2_sets_test-1;
				
						setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
						tagbitshift= L2_s_bits+b_bits;
				
						tag_sumsize=pow(2,L2_tag_bits);
				
						tag_mask=tag_sumsize-1;
				
						tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
						holder= &L2_set_ass[setassoc_i][0];
				
						for (int i=0; i<L2_numlines;i++)
						{
							if (L2_set_ass[setassoc_i][i].valid_bit==1)
							{
								num_elements++;
							}
						}
						
						remaining_space=L2_numlines-num_elements;
				
						while (counter>0)
						{
							if (holder->tag_bits==tag_index)
							{
								cold_flag=1;
								break;
							}
					
							else
							{
								searchi++;
						
								if (&L2_set_ass[setassoc_i][searchi]!=NULL)
								{
									holder=&L2_set_ass[setassoc_i][searchi];
								}
							}
					
							counter--;
						}
				
						if (cold_flag==1)
						{
							L2_total_hits++;
					
							if (policy_flag==2)
							{
								setLRUhit(tag_index, 2,setassoc_i);
							}
						}
				
						else if (cold_flag==0)
						{
							L2_total_misses++;
									
							if(remaining_space!=0)
							{
								searchi=0;
								holder=&L2_set_ass[setassoc_i][0];
								
								while (holder->valid_bit!=0)
								{
									searchi++;
					
									if (&L2_set_ass[setassoc_i][searchi]!=NULL)
									{
										holder=&L2_set_ass[setassoc_i][searchi];
									}
								}
										
								holder->valid_bit=1;
								holder->tag_bits= tag_index;
								L2_cold_misses++;
								setenq(holder,setassoc_i,2); 
							}
							
							else if(remaining_space==0)
							{
								long long tag_replace=0;
										
								tag_replace=setdequeue(2,setassoc_i);
										
								LINE *guide=&L2_set_ass[setassoc_i][0];
								int a_index=0;
										
								while (guide->tag_bits!=tag_replace)
								{
									a_index++;
											
									if (&L2_set_ass[setassoc_i][a_index]!=NULL)
									{
										guide=&L2_set_ass[setassoc_i][a_index];
									}
								}
										
								guide->tag_bits=tag_index;
										
								setenq(guide, setassoc_i,2);
							}			
						}
					}
				}
			}
		}
		
		if(argv_length==15) /* 3 cache simulation */
		{
				
			if (strcmp(argv[4],direct)==0)
			{
				temp_hold=L1_total_misses;
				temp_hold_hit=L1_total_hits;
				temp_cold_hold=L1_cold_misses;
				
				function_result=directmap(hex_value, L1_set_list, L1_s_bits, b_bits, L1_tag_bits, L1_total_misses, L1_total_hits, L1_cold_misses);
				
				if(function_result==1)
				{
					L1_cold_misses++;
				}
				
				if(function_result==3)
				{
					L1_total_hits++;
				}
				else
				{
					L1_total_misses++;
				}
				
				 /* must simulate full associative cache.*/
				
					L1_sim_tagbits= m_bits-(b_bits);
					
					function_result=fullassoc(hex_value, L1_sim, b_bits, L1_sim_size, L1_sim_cold, L1_sim_capacity, L1_sim_misses, 4,L1_sim_tagbits,L1_sim_hits, 2);
					
					if(function_result==1)
					{
						L1_sim_cold++;
					}
				
					if(function_result==3)
					{
						L1_sim_hits++;
					}
					else
					{
						L1_sim_misses++;
					}
					
				if (L1_total_misses>temp_hold) /* go to 2nd cache */ 
				{ 
					if (strcmp(argv[8],direct)==0)
					{
						temp_hold= L2_total_misses;
						temp_cold_hold= L2_cold_misses;
						
						function_result= directmap(hex_value, L2_set_list, L2_s_bits, b_bits, L2_tag_bits, L2_total_misses, L2_total_hits, L2_cold_misses);
						
						if(function_result==1)
						{
							L2_cold_misses++;
						}
				
						if(function_result==3)
						{
							L2_total_hits++;
						}
						else
						{
							L2_total_misses++;
						}
						
						if (L2_total_misses>temp_hold) // go to third cache
						{
							if (strcmp(argv[12],direct)==0)
							{
								function_result=directmap(hex_value, L3_set_list, L3_s_bits, b_bits, L3_tag_bits, L3_total_misses, L3_total_hits, L3_cold_misses);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
								else
								{
									L3_total_misses++;
								}
							}
							
							else if (strcmp(argv[12],assoc)==0) /* full associative*/
							{
								function_result=fullassoc(hex_value, L3_line_list, b_bits, L3_numlines, L3_cold_misses,L3_capacity_misses, L3_total_misses, 3, L3_tag_bits,L3_total_hits, policy_flag);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
							
								else
								{	
									L3_total_misses++;
								}
							}
							else if (leng3>5 && (strcmp(argv[12],direct)!=0)) /* set associative */
							{
								counter= L3_numlines;
				
								searchi=0;
				
								num_elements=0;
				
								fullsize=(three_size)/(block_size);
				
								cold_flag=0;
				
								setassoc_mask= L3_sets_test-1;
				
								setassoc_i=(hex_value >> b_bits) & setassoc_mask;
								
								tagbitshift= L3_s_bits+b_bits;
				
								tag_sumsize=pow(2,L3_tag_bits);
				
								tag_mask=tag_sumsize-1;
				
								tag_index= (hex_value >> tagbitshift) & tag_mask; 
								
								holder= &L3_set_ass[setassoc_i][0];
								
								for (int i=0; i<L3_numlines;i++)
								{
									if (L3_set_ass[setassoc_i][i].valid_bit==1)
									{
										num_elements++;
									}
								}
								remaining_space=L3_numlines-num_elements;
				
								while (counter>0)
								{
									if (holder->tag_bits==tag_index)
									{
										cold_flag=1;
										break;
									}
					
									else
									{
										searchi++;
						
										if (&L3_set_ass[setassoc_i][searchi]!=NULL)
										{
											holder=&L3_set_ass[setassoc_i][searchi];
										}
									}
					
									counter--;
								}
				
								if (cold_flag==1)
								{
									L3_total_hits++;
					
									if (policy_flag==2)
									{
										setLRUhit(tag_index, 3,setassoc_i);
									}
								}
				
								else if (cold_flag==0)
								{
									L3_total_misses++;
									
									if(remaining_space!=0)
									{
										searchi=0;
										holder=&L3_set_ass[setassoc_i][0];
										while (holder->valid_bit!=0)
										{
											searchi++;
					
											if (&L3_set_ass[setassoc_i][searchi]!=NULL)
											{
												holder=&L3_set_ass[setassoc_i][searchi];
											}
										}
										
										holder->valid_bit=1;
										holder->tag_bits= tag_index;
										L3_cold_misses++;
										setenq(holder,setassoc_i,3); 
									}
									else if(remaining_space==0)
									{
										long long tag_replace=0;
										
										tag_replace=setdequeue(3,setassoc_i);
										
										LINE *guide=&L3_set_ass[setassoc_i][0];
										int a_index=0;
										
										while (guide->tag_bits!=tag_replace)
										{
											a_index++;
											
											if (&L3_set_ass[setassoc_i][a_index]!=NULL)
											{
												guide=&L3_set_ass[setassoc_i][a_index];
											}
										}
										
										guide->tag_bits=tag_index;
										
										setenq(guide, setassoc_i,3);
									}
									
								}
							}
						}
					}
					
					else if (strcmp(argv[8],assoc)==0) /* full associative*/
					{
						temp_hold= L2_total_misses;
						temp_cold_hold= L2_cold_misses;
						
						function_result=fullassoc(hex_value, L2_line_list, b_bits, L2_numlines, L2_cold_misses,L2_capacity_misses, L2_total_misses, 2, L2_tag_bits, L2_total_hits,policy_flag);
						
						if(function_result==1)
						{
							L2_cold_misses++;
						}
				
						if(function_result==3)
						{
							L2_total_hits++;
						}
						
						else
						{
							L2_total_misses++;
						}
						
						if (L2_total_misses>temp_hold) // go to third cache
						{
							if (strcmp(argv[12],direct)==0)
							{
								function_result=directmap(hex_value, L3_set_list, L3_s_bits, b_bits, L3_tag_bits, L3_total_misses, L3_total_hits, L3_cold_misses);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
								else
								{
									L3_total_misses++;
								}
							}
							
							else if (strcmp(argv[12],assoc)==0) /* full associative*/
							{
								function_result=fullassoc(hex_value, L3_line_list, b_bits, L3_numlines, L3_cold_misses,L3_capacity_misses, L3_total_misses, 3, L3_tag_bits,L3_total_hits, policy_flag);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
							
								else
								{	
									L3_total_misses++;
								}
							}
							
							else if (leng3>5 && (strcmp(argv[12],direct)!=0)) /* set associative */
							{
								counter= L3_numlines;
				
								searchi=0;
				
								num_elements=0;
				
								fullsize=(three_size)/(block_size);
				
								cold_flag=0;
				
								setassoc_mask= L3_sets_test-1;
				
								setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
								tagbitshift= L3_s_bits+b_bits;
				
								tag_sumsize=pow(2,L3_tag_bits);
				
								tag_mask=tag_sumsize-1;
				
								tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
								holder= &L3_set_ass[setassoc_i][0];
				
								for (int i=0; i<L3_numlines;i++)
								{
									if (L3_set_ass[setassoc_i][i].valid_bit==1)
									{
										num_elements++;
									}
								}
								remaining_space=L3_numlines-num_elements;
				
								while (counter>0)
								{
									if (holder->tag_bits==tag_index)
									{
										cold_flag=1;
										break;
									}
					
									else
									{
										searchi++;
						
										if (&L3_set_ass[setassoc_i][searchi]!=NULL)
										{
											holder=&L3_set_ass[setassoc_i][searchi];
										}
									}
					
									counter--;
								}
				
								if (cold_flag==1)
								{
									L3_total_hits++;
					
									if (policy_flag==2)
									{
										setLRUhit(tag_index, 3,setassoc_i);
									}
								}
				
								else if (cold_flag==0)
								{
									L3_total_misses++;
									
									if(remaining_space!=0)
									{
										searchi=0;
										holder=&L3_set_ass[setassoc_i][0];
										while (holder->valid_bit!=0)
										{
											searchi++;
					
											if (&L3_set_ass[setassoc_i][searchi]!=NULL)
											{
												holder=&L3_set_ass[setassoc_i][searchi];
											}
										}
										
										holder->valid_bit=1;
										holder->tag_bits= tag_index;
										L3_cold_misses++;
										setenq(holder,setassoc_i,3); 
									}
									else if(remaining_space==0)
									{
										long long tag_replace=0;
										
										tag_replace=setdequeue(3,setassoc_i);
										
										LINE *guide=&L3_set_ass[setassoc_i][0];
										int a_index=0;
										
										while (guide->tag_bits!=tag_replace)
										{
											a_index++;
											
											if (&L3_set_ass[setassoc_i][a_index]!=NULL)
											{
												guide=&L3_set_ass[setassoc_i][a_index];
											}
										}
										
										guide->tag_bits=tag_index;
										
										setenq(guide, setassoc_i,3);
									}
									
								}
							}
						}
					}
					
					else if (leng2>5 && (strcmp(argv[8],direct)!=0)) /* set associative */
					{
						temp_hold_hit=L2_total_hits;
				
						temp_hold=L2_total_misses;
				
						temp_cold_hold=L2_cold_misses;
				
						counter= L2_numlines;
				
						searchi=0;
				
						num_elements=0;
				
						fullsize=(two_size)/(block_size);
				
						cold_flag=0;
				
						setassoc_mask= L2_sets_test-1;
				
						setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
						tagbitshift= L2_s_bits+b_bits;
				
						tag_sumsize=pow(2,L2_tag_bits);
				
						tag_mask=tag_sumsize-1;
				
						tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
						holder= &L2_set_ass[setassoc_i][0];
				
						for (int i=0; i<L2_numlines;i++)
						{
							if (L2_set_ass[setassoc_i][i].valid_bit==1)
							{
								num_elements++;
							}
						}
						
						remaining_space=L2_numlines-num_elements;
				
						while (counter>0)
						{
							if (holder->tag_bits==tag_index)
							{
								cold_flag=1;
								break;
							}
					
							else
							{
								searchi++;
						
								if (&L2_set_ass[setassoc_i][searchi]!=NULL)
								{
									holder=&L2_set_ass[setassoc_i][searchi];
								}
							}
					
							counter--;
						}
				
						if (cold_flag==1)
						{
							L2_total_hits++;
					
							if (policy_flag==2)
							{
								setLRUhit(tag_index, 2,setassoc_i);
							}
						}
				
						else if (cold_flag==0)
						{
							L2_total_misses++;
									
							if(remaining_space!=0)
							{
								searchi=0;
								holder=&L2_set_ass[setassoc_i][0];
								
								while (holder->valid_bit!=0)
								{
									searchi++;
					
									if (&L2_set_ass[setassoc_i][searchi]!=NULL)
									{
										holder=&L2_set_ass[setassoc_i][searchi];
									}
								}
										
								holder->valid_bit=1;
								holder->tag_bits= tag_index;
								L2_cold_misses++;
								setenq(holder,setassoc_i,2); 
							}
							
							else if(remaining_space==0)
							{
								long long tag_replace=0;
										
								tag_replace=setdequeue(2,setassoc_i);
										
								LINE *guide=&L2_set_ass[setassoc_i][0];
								int a_index=0;
										
								while (guide->tag_bits!=tag_replace)
								{
									a_index++;
											
									if (&L2_set_ass[setassoc_i][a_index]!=NULL)
									{
										guide=&L2_set_ass[setassoc_i][a_index];
									}
								}
										
								guide->tag_bits=tag_index;
										
								setenq(guide, setassoc_i,2);
							}
									
						}
						
						if (L2_total_misses>temp_hold) /* go to third cache */
						{
							if (strcmp(argv[12],direct)==0)
							{
								function_result=directmap(hex_value, L3_set_list, L3_s_bits, b_bits, L3_tag_bits, L3_total_misses, L3_total_hits, L3_cold_misses);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
								else
								{
									L3_total_misses++;
								}
							}
							else if (strcmp(argv[12],assoc)==0) /* full associative*/
							{
								function_result=fullassoc(hex_value, L3_line_list, b_bits, L3_numlines, L3_cold_misses,L3_capacity_misses, L3_total_misses, 3, L3_tag_bits,L3_total_hits, policy_flag);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
							
								else
								{	
									L3_total_misses++;
								}
							}
							else if (leng3>5 && (strcmp(argv[12],direct)!=0)) /* set associative */
							{
								counter= L3_numlines;
				
								searchi=0;
				
								num_elements=0;
				
								fullsize=(three_size)/(block_size);
				
								cold_flag=0;
				
								setassoc_mask= L3_sets_test-1;
				
								setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
								tagbitshift= L3_s_bits+b_bits;
				
								tag_sumsize=pow(2,L3_tag_bits);
				
								tag_mask=tag_sumsize-1;
				
								tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
								holder= &L3_set_ass[setassoc_i][0];
				
								for (int i=0; i<L3_numlines;i++)
								{
									if (L3_set_ass[setassoc_i][i].valid_bit==1)
									{
										num_elements++;
									}
								}
								remaining_space=L3_numlines-num_elements;
				
								while (counter>0)
								{
									if (holder->tag_bits==tag_index)
									{
										cold_flag=1;
										break;
									}
					
									else
									{
										searchi++;
						
										if (&L3_set_ass[setassoc_i][searchi]!=NULL)
										{
											holder=&L3_set_ass[setassoc_i][searchi];
										}
									}
					
									counter--;
								}
				
								if (cold_flag==1)
								{
									L3_total_hits++;
					
									if (policy_flag==2)
									{
										setLRUhit(tag_index, 3,setassoc_i);
									}
								}
				
								else if (cold_flag==0)
								{
									L3_total_misses++;
									
									if(remaining_space!=0)
									{
										searchi=0;
										holder=&L3_set_ass[setassoc_i][0];
										while (holder->valid_bit!=0)
										{
											searchi++;
					
											if (&L3_set_ass[setassoc_i][searchi]!=NULL)
											{
												holder=&L3_set_ass[setassoc_i][searchi];
											}
										}
										
										holder->valid_bit=1;
										holder->tag_bits= tag_index;
										L3_cold_misses++;
										setenq(holder,setassoc_i,3); 
									}
									else if(remaining_space==0)
									{
										long long tag_replace=0;
										
										tag_replace=setdequeue(3,setassoc_i);
										
										LINE *guide=&L3_set_ass[setassoc_i][0];
										int a_index=0;
										
										while (guide->tag_bits!=tag_replace)
										{
											a_index++;
											
											if (&L3_set_ass[setassoc_i][a_index]!=NULL)
											{
												guide=&L3_set_ass[setassoc_i][a_index];
											}
										}
										
										guide->tag_bits=tag_index;
										
										setenq(guide, setassoc_i,3);
									}
									
								}
							}
						}	
					}
				}
			}
			
			else if (strcmp(argv[4],assoc)==0)
			{	
				temp_hold=L1_total_misses;
				temp_cold_hold=L1_cold_misses;
				
				function_result=fullassoc(hex_value, L1_line_list, b_bits, L1_numlines, L1_cold_misses,L1_capacity_misses, L1_total_misses,1,L1_tag_bits,L1_total_hits,policy_flag);
				
				if(function_result==1)
				{
					L1_cold_misses++;
					
				}
				
				if(function_result==3)
				{
					L1_total_hits++;
				}
				else
				{
					L1_total_misses++;
				}
				
				if (L1_total_misses>temp_hold) /* go to 2nd cache */ 
				{
					
					if (strcmp(argv[8],direct)==0)
					{
						temp_hold= L2_total_misses;
						temp_cold_hold= L2_cold_misses;
						
						function_result=directmap(hex_value, L2_set_list, L2_s_bits, b_bits, L2_tag_bits, L2_total_misses, L2_total_hits, L2_cold_misses);
						
						if(function_result==1)
						{
							L2_cold_misses++;
							
						}
				
						if(function_result==3)
						{
							L2_total_hits++;
						}
						
						else
						{
							L2_total_misses++;
						}
						
						if (L2_total_misses>temp_hold) // go to third cache
						{
							if (strcmp(argv[12],direct)==0)
							{
								function_result=directmap(hex_value, L3_set_list, L3_s_bits, b_bits, L3_tag_bits, L3_total_misses, L3_total_hits, L3_cold_misses);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
								else
								{
									L3_total_misses++;
								}
							}
							
							else if (strcmp(argv[12],assoc)==0) /* full associative*/
							{
								function_result=fullassoc(hex_value, L3_line_list, b_bits, L3_numlines, L3_cold_misses,L3_capacity_misses, L3_total_misses, 3, L3_tag_bits,L3_total_hits, policy_flag);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
							
								else
								{	
									L3_total_misses++;
								}
							}
							else if (leng3>5 && (strcmp(argv[12],direct)!=0)) /* set associative */
							{
								counter= L3_numlines;
				
								searchi=0;
				
								num_elements=0;
				
								fullsize=(three_size)/(block_size);
				
								cold_flag=0;
				
								setassoc_mask= L3_sets_test-1;
				
								setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
								tagbitshift= L3_s_bits+b_bits;
				
								tag_sumsize=pow(2,L3_tag_bits);
				
								tag_mask=tag_sumsize-1;
				
								tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
								holder= &L3_set_ass[setassoc_i][0];
				
								for (int i=0; i<L3_numlines;i++)
								{
									if (L3_set_ass[setassoc_i][i].valid_bit==1)
									{
										num_elements++;
									}
								}
								remaining_space=L3_numlines-num_elements;
				
								while (counter>0)
								{
									if (holder->tag_bits==tag_index)
									{
										cold_flag=1;
										break;
									}
					
									else
									{
										searchi++;
						
										if (&L3_set_ass[setassoc_i][searchi]!=NULL)
										{
											holder=&L3_set_ass[setassoc_i][searchi];
										}
									}
					
									counter--;
								}
				
								if (cold_flag==1)
								{
									L3_total_hits++;
					
									if (policy_flag==2)
									{
										setLRUhit(tag_index, 3,setassoc_i);
									}
								}
				
								else if (cold_flag==0)
								{
									L3_total_misses++;
									
									if(remaining_space!=0)
									{
										searchi=0;
										holder=&L3_set_ass[setassoc_i][0];
										while (holder->valid_bit!=0)
										{
											searchi++;
					
											if (&L3_set_ass[setassoc_i][searchi]!=NULL)
											{
												holder=&L3_set_ass[setassoc_i][searchi];
											}
										}
										
										holder->valid_bit=1;
										holder->tag_bits= tag_index;
										L3_cold_misses++;
										setenq(holder,setassoc_i,3); 
									}
									else if(remaining_space==0)
									{
										long long tag_replace=0;
										
										tag_replace=setdequeue(3,setassoc_i);
										
										LINE *guide=&L3_set_ass[setassoc_i][0];
										int a_index=0;
										
										while (guide->tag_bits!=tag_replace)
										{
											a_index++;
											
											if (&L3_set_ass[setassoc_i][a_index]!=NULL)
											{
												guide=&L3_set_ass[setassoc_i][a_index];
											}
										}
										
										guide->tag_bits=tag_index;
										
										setenq(guide, setassoc_i,3);
									}	
								}
							}
						}
					}
					
					else if (strcmp(argv[8],assoc)==0) /* full associative*/
					{
						temp_hold= L2_total_misses;
						temp_cold_hold= L2_cold_misses;
						
						function_result=fullassoc(hex_value, L2_line_list, b_bits, L2_numlines, L2_cold_misses,L2_capacity_misses, L2_total_misses, 2, L2_tag_bits, L2_total_hits,policy_flag);
						
						if(function_result==1)
						{
							L2_cold_misses++;
							
						}
				
						if(function_result==3)
						{
							L2_total_hits++;
						}
						
						else
						{
							L2_total_misses++;
						}
						
						if (L2_total_misses>temp_hold) // go to third cache
						{
							if (strcmp(argv[12],direct)==0)
							{
								function_result=directmap(hex_value, L3_set_list, L3_s_bits, b_bits, L3_tag_bits, L3_total_misses, L3_total_hits, L3_cold_misses);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
								else
								{
									L3_total_misses++;
								}
							}
							else if (strcmp(argv[12],assoc)==0) /* full associative*/
							{
								function_result=fullassoc(hex_value, L3_line_list, b_bits, L3_numlines, L3_cold_misses,L3_capacity_misses, L3_total_misses, 3, L3_tag_bits,L3_total_hits, policy_flag);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
							
								else
								{	
									L3_total_misses++;
								}
							}
							else if (leng3>5 && (strcmp(argv[12],direct)!=0)) /* set associative */
							{
								counter= L3_numlines;
				
								searchi=0;
				
								num_elements=0;
				
								fullsize=(three_size)/(block_size);
				
								cold_flag=0;
				
								setassoc_mask= L3_sets_test-1;
				
								setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
								tagbitshift= L3_s_bits+b_bits;
				
								tag_sumsize=pow(2,L3_tag_bits);
				
								tag_mask=tag_sumsize-1;
				
								tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
								holder= &L3_set_ass[setassoc_i][0];
								
								for (int i=0; i<L3_numlines;i++)
								{
									if (L3_set_ass[setassoc_i][i].valid_bit==1)
									{
										num_elements++;
									}
								}
								remaining_space=L3_numlines-num_elements;
				
								while (counter>0)
								{
									if (holder->tag_bits==tag_index)
									{
										cold_flag=1;
										break;
									}
					
									else
									{
										searchi++;
						
										if (&L3_set_ass[setassoc_i][searchi]!=NULL)
										{
											holder=&L3_set_ass[setassoc_i][searchi];
										}
									}
					
									counter--;
								}
				
								if (cold_flag==1)
								{
									L3_total_hits++;
					
									if (policy_flag==2)
									{
										setLRUhit(tag_index, 3,setassoc_i);
									}
								}
				
								else if (cold_flag==0)
								{
									L3_total_misses++;
									
									if(remaining_space!=0)
									{
										searchi=0;
										holder=&L3_set_ass[setassoc_i][0];
										while (holder->valid_bit!=0)
										{
											searchi++;
					
											if (&L3_set_ass[setassoc_i][searchi]!=NULL)
											{
												holder=&L3_set_ass[setassoc_i][searchi];
											}
										}
										
										holder->valid_bit=1;
										holder->tag_bits= tag_index;
										L3_cold_misses++;
										setenq(holder,setassoc_i,3); 
									}
									else if(remaining_space==0)
									{
										long long tag_replace=0;
										
										tag_replace=setdequeue(3,setassoc_i);
										
										LINE *guide=&L3_set_ass[setassoc_i][0];
										int a_index=0;
										
										while (guide->tag_bits!=tag_replace)
										{
											a_index++;
											
											if (&L3_set_ass[setassoc_i][a_index]!=NULL)
											{
												guide=&L3_set_ass[setassoc_i][a_index];
											}
										}
										
										guide->tag_bits=tag_index;
										
										setenq(guide, setassoc_i,3);
									}
								}
							}
						}
					}
					else if (leng2>5 && (strcmp(argv[8],direct)!=0)) /* set associative */
					{
						temp_hold_hit=L2_total_hits;
				
						temp_hold=L2_total_misses;
				
						temp_cold_hold=L2_cold_misses;
				
						counter= L2_numlines;
				
						searchi=0;
				
						num_elements=0;
				
						fullsize=(two_size)/(block_size);
				
						cold_flag=0;
				
						setassoc_mask= L2_sets_test-1;
				
						setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
						tagbitshift= L2_s_bits+b_bits;
				
						tag_sumsize=pow(2,L2_tag_bits);
				
						tag_mask=tag_sumsize-1;
				
						tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
						holder= &L2_set_ass[setassoc_i][0];
				
							for (int i=0; i<L2_numlines;i++)
						{
							if (L2_set_ass[setassoc_i][i].valid_bit==1)
							{
								num_elements++;
							}
						}
						
						remaining_space=L2_numlines-num_elements;
				
						while (counter>0)
						{
							if (holder->tag_bits==tag_index)
							{
								cold_flag=1;
								break;
							}
					
							else
							{
								searchi++;
						
								if (&L2_set_ass[setassoc_i][searchi]!=NULL)
								{
									holder=&L2_set_ass[setassoc_i][searchi];
								}
							}
					
							counter--;
						}
				
						if (cold_flag==1)
						{
							L2_total_hits++;
					
							if (policy_flag==2)
							{
								setLRUhit(tag_index, 2,setassoc_i);
							}
						}
				
						else if (cold_flag==0)
						{
							L2_total_misses++;
									
							if(remaining_space!=0)
							{
								searchi=0;
								holder=&L2_set_ass[setassoc_i][0];
								
								while (holder->valid_bit!=0)
								{
									searchi++;
					
									if (&L2_set_ass[setassoc_i][searchi]!=NULL)
									{
										holder=&L2_set_ass[setassoc_i][searchi];
									}
								}
										
								holder->valid_bit=1;
								holder->tag_bits= tag_index;
								L2_cold_misses++;
								setenq(holder,setassoc_i,2); 
							}
							
							else if(remaining_space==0)
							{
								long long tag_replace=0;
										
								tag_replace=setdequeue(2,setassoc_i);
										
								LINE *guide=&L2_set_ass[setassoc_i][0];
								int a_index=0;
										
								while (guide->tag_bits!=tag_replace)
								{
									a_index++;
											
									if (&L2_set_ass[setassoc_i][a_index]!=NULL)
									{
										guide=&L2_set_ass[setassoc_i][a_index];
									}
								}
										
								guide->tag_bits=tag_index;
										
								setenq(guide, setassoc_i,2);
							}
									
						}
						if (L2_total_misses>temp_hold) /* go to third cache */
						{
							if (strcmp(argv[12],direct)==0)
							{
								function_result=directmap(hex_value, L3_set_list, L3_s_bits, b_bits, L3_tag_bits, L3_total_misses, L3_total_hits, L3_cold_misses);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
								else
								{
									L3_total_misses++;
								}
							}
							else if (strcmp(argv[12],assoc)==0) /* full associative*/
							{
								function_result=fullassoc(hex_value, L3_line_list, b_bits, L3_numlines, L3_cold_misses,L3_capacity_misses, L3_total_misses, 3, L3_tag_bits,L3_total_hits, policy_flag);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
							
								else
								{	
									L3_total_misses++;
								}
							}
							else if (leng3>5 && (strcmp(argv[12],direct)!=0)) /* set associative */
							{
								counter= L3_numlines;
			
								searchi=0;
				
								num_elements=0;
				
								fullsize=(three_size)/(block_size);
				
								cold_flag=0;
				
								setassoc_mask= L3_sets_test-1;
				
								setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
								tagbitshift= L3_s_bits+b_bits;
				
								tag_sumsize=pow(2,L3_tag_bits);
				
								tag_mask=tag_sumsize-1;
				
								tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
								holder= &L3_set_ass[setassoc_i][0];
				
								for (int i=0; i<L3_numlines;i++)
								{
									if (L3_set_ass[setassoc_i][i].valid_bit==1)
									{
										num_elements++;
									}
								}
								remaining_space=L3_numlines-num_elements;
				
								while (counter>0)
								{
									if (holder->tag_bits==tag_index)
									{
										cold_flag=1;
										break;
									}
					
									else
									{
										searchi++;
						
										if (&L3_set_ass[setassoc_i][searchi]!=NULL)
										{
											holder=&L3_set_ass[setassoc_i][searchi];
										}
									}
					
									counter--;
								}
				
								if (cold_flag==1)
								{
									L3_total_hits++;
					
									if (policy_flag==2)
									{
										setLRUhit(tag_index, 3,setassoc_i);
									}
								}
				
								else if (cold_flag==0)
								{
									L3_total_misses++;
									
									if(remaining_space!=0)
									{
										searchi=0;
										holder=&L3_set_ass[setassoc_i][0];
										while (holder->valid_bit!=0)
										{
											searchi++;
					
											if (&L3_set_ass[setassoc_i][searchi]!=NULL)
											{
												holder=&L3_set_ass[setassoc_i][searchi];
											}
										}
										
										holder->valid_bit=1;
										holder->tag_bits= tag_index;
										L3_cold_misses++;
										setenq(holder,setassoc_i,3); 
									}
									else if(remaining_space==0)
									{
										long long tag_replace=0;
										
										tag_replace=setdequeue(3,setassoc_i);
										
										LINE *guide=&L3_set_ass[setassoc_i][0];
										int a_index=0;
										
										while (guide->tag_bits!=tag_replace)
										{
											a_index++;
											
											if (&L3_set_ass[setassoc_i][a_index]!=NULL)
											{
												guide=&L3_set_ass[setassoc_i][a_index];
											}
										}
										
										guide->tag_bits=tag_index;
										
										setenq(guide, setassoc_i,3);
									}
								}
							}
						}
					}
				}
			}
			else if (leng>5 && (strcmp(argv[4],direct)!=0))
			{
				temp_hold_hit=L1_total_hits;
				
				temp_hold=L1_total_misses;
				
				temp_cold_hold=L1_cold_misses;
				
				counter= L1_numlines;
				
				searchi=0;
				
				num_elements=0;
				
				fullsize=(one_size)/(block_size);
				
				cold_flag=0;
				
				setassoc_mask= L1_sets_test-1;
				
				setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
				tagbitshift= L1_s_bits+b_bits;
				
				tag_sumsize=pow(2,L1_tag_bits);
				
				tag_mask=tag_sumsize-1;
				
				tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
				holder= &L1_set_ass[setassoc_i][0];
				
				for (int i=0; i<L1_numlines;i++)
				{
					if (L1_set_ass[setassoc_i][i].valid_bit==1)
					{
						num_elements++;
					}
				}
						
				remaining_space=L1_numlines-num_elements;
				
				while (counter>0)
				{
					if (holder->tag_bits==tag_index)
					{
						cold_flag=1;
						break;
					}
					
					else
					{
						searchi++;
						
						if (&L1_set_ass[setassoc_i][searchi]!=NULL)
						{
							holder=&L1_set_ass[setassoc_i][searchi];
						}
					}
					
					counter--;
				}
				
				if (cold_flag==1)
				{
					L1_total_hits++;
					
					if (policy_flag==2)
					{
						setLRUhit(tag_index, 1,setassoc_i);
					}
				}
				
				else if (cold_flag==0)
				{
					L1_total_misses++;
								
					if(remaining_space!=0)
					{
						searchi=0;
						holder=&L1_set_ass[setassoc_i][0];
								
						while (holder->valid_bit!=0)
						{
							searchi++;
					
							if (&L1_set_ass[setassoc_i][searchi]!=NULL)
							{
								holder=&L1_set_ass[setassoc_i][searchi];
							}
						}
										
						holder->valid_bit=1;
						holder->tag_bits= tag_index;
						L1_cold_misses++;
						setenq(holder,setassoc_i,1); 
					}
							
					else if(remaining_space==0)
					{
						long long tag_replace=0;
										
						tag_replace=setdequeue(1,setassoc_i);
										
						LINE *guide=&L1_set_ass[setassoc_i][0];
						int a_index=0;
										
						while (guide->tag_bits!=tag_replace)
						{
							a_index++;
											
							if (&L1_set_ass[setassoc_i][a_index]!=NULL)
							{
								guide=&L1_set_ass[setassoc_i][a_index];
							}
						}	
							
						guide->tag_bits=tag_index;
										
						setenq(guide, setassoc_i,1);
					}
									
				}
				/* simulate full associative */
				
					L1_sim_tagbits= m_bits-(b_bits);
					
					function_result=fullassoc(hex_value, L1_sim, b_bits, L1_sim_size, L1_sim_cold, L1_sim_capacity, L1_sim_misses, 4,L1_sim_tagbits,L1_sim_hits, 2);
					
					if(function_result==1)
					{
						L1_sim_cold++;
					}
				
					if(function_result==3)
					{
						L1_sim_hits++;
					}
					else
					{
						L1_sim_misses++;
					}
				
				if (L1_total_misses>temp_hold) /* go to second cache */
				{
					if (strcmp(argv[8],direct)==0)
					{
						temp_hold= L2_total_misses;
						temp_cold_hold= L2_cold_misses;
						
						function_result= directmap(hex_value, L2_set_list, L2_s_bits, b_bits, L2_tag_bits, L2_total_misses, L2_total_hits, L2_cold_misses);
						
						if(function_result==1)
						{
							L2_cold_misses++;
						}
				
						if(function_result==3)
						{
							L2_total_hits++;
						}
						else
						{
							L2_total_misses++;
						}
						
						if (L2_total_misses>temp_hold) // go to third cache
						{
							if (strcmp(argv[12],direct)==0)
							{
								function_result=directmap(hex_value, L3_set_list, L3_s_bits, b_bits, L3_tag_bits, L3_total_misses, L3_total_hits, L3_cold_misses);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
								else
								{
									L3_total_misses++;
								}
							}
							
							else if (strcmp(argv[12],assoc)==0) /* full associative*/
							{
								function_result=fullassoc(hex_value, L3_line_list, b_bits, L3_numlines, L3_cold_misses,L3_capacity_misses, L3_total_misses, 3, L3_tag_bits,L3_total_hits, policy_flag);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
							
								else
								{	
									L3_total_misses++;
								}
							}
							else if (leng3>5 && (strcmp(argv[12],direct)!=0)) /* set associative */
							{
								counter= L3_numlines;
				
								searchi=0;
				
								num_elements=0;
				
								fullsize=(three_size)/(block_size);
				
								cold_flag=0;
				
								setassoc_mask= L3_sets_test-1;
				
								setassoc_i=(hex_value >> b_bits) & setassoc_mask;
								
								tagbitshift= L3_s_bits+b_bits;
				
								tag_sumsize=pow(2,L3_tag_bits);
				
								tag_mask=tag_sumsize-1;
				
								tag_index= (hex_value >> tagbitshift) & tag_mask; 
								
								holder= &L3_set_ass[setassoc_i][0];
								
								for (int i=0; i<L3_numlines;i++)
								{
									if (L3_set_ass[setassoc_i][i].valid_bit==1)
									{
										num_elements++;
									}
								}
								remaining_space=L3_numlines-num_elements;
				
								while (counter>0)
								{
									if (holder->tag_bits==tag_index)
									{
										cold_flag=1;
										break;
									}
					
									else
									{
										searchi++;
						
										if (&L3_set_ass[setassoc_i][searchi]!=NULL)
										{
											holder=&L3_set_ass[setassoc_i][searchi];
										}
									}
					
									counter--;
								}
				
								if (cold_flag==1)
								{
									L3_total_hits++;
					
									if (policy_flag==2)
									{
										setLRUhit(tag_index, 3,setassoc_i);
									}
								}
				
								else if (cold_flag==0)
								{
									L3_total_misses++;
									
									if(remaining_space!=0)
									{
										searchi=0;
										holder=&L3_set_ass[setassoc_i][0];
										while (holder->valid_bit!=0)
										{
											searchi++;
					
											if (&L3_set_ass[setassoc_i][searchi]!=NULL)
											{
												holder=&L3_set_ass[setassoc_i][searchi];
											}
										}
										
										holder->valid_bit=1;
										holder->tag_bits= tag_index;
										L3_cold_misses++;
										setenq(holder,setassoc_i,3); 
									}
									else if(remaining_space==0)
									{
										long long tag_replace=0;
										
										tag_replace=setdequeue(3,setassoc_i);
										
										LINE *guide=&L3_set_ass[setassoc_i][0];
										int a_index=0;
										
										while (guide->tag_bits!=tag_replace)
										{
											a_index++;
											
											if (&L3_set_ass[setassoc_i][a_index]!=NULL)
											{
												guide=&L3_set_ass[setassoc_i][a_index];
											}
										}
										
										guide->tag_bits=tag_index;
										
										setenq(guide, setassoc_i,3);
									}
									
								}
							}
						}
					}
					else if (strcmp(argv[8],assoc)==0) /* full associative*/
					{
						temp_hold= L2_total_misses;
						temp_cold_hold= L2_cold_misses;
						
						function_result=fullassoc(hex_value, L2_line_list, b_bits, L2_numlines, L2_cold_misses,L2_capacity_misses, L2_total_misses, 2, L2_tag_bits, L2_total_hits,policy_flag);
						
						if(function_result==1)
						{
							L2_cold_misses++;
						}
				
						if(function_result==3)
						{
							L2_total_hits++;
						}
						
						else
						{
							L2_total_misses++;
						}
						
						if (L2_total_misses>temp_hold) // go to third cache
						{
							if (strcmp(argv[12],direct)==0)
							{
								function_result=directmap(hex_value, L3_set_list, L3_s_bits, b_bits, L3_tag_bits, L3_total_misses, L3_total_hits, L3_cold_misses);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
								else
								{
									L3_total_misses++;
								}
							}
							
							else if (strcmp(argv[12],assoc)==0) /* full associative*/
							{
								function_result=fullassoc(hex_value, L3_line_list, b_bits, L3_numlines, L3_cold_misses,L3_capacity_misses, L3_total_misses, 3, L3_tag_bits,L3_total_hits, policy_flag);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
							
								else
								{	
									L3_total_misses++;
								}
							}
							
							else if (leng3>5 && (strcmp(argv[12],direct)!=0)) /* set associative */
							{
								counter= L3_numlines;
				
								searchi=0;
				
								num_elements=0;
				
								fullsize=(three_size)/(block_size);
				
								cold_flag=0;
				
								setassoc_mask= L3_sets_test-1;
				
								setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
								tagbitshift= L3_s_bits+b_bits;
				
								tag_sumsize=pow(2,L3_tag_bits);
				
								tag_mask=tag_sumsize-1;
				
								tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
								holder= &L3_set_ass[setassoc_i][0];
				
								for (int i=0; i<L3_numlines;i++)
								{
									if (L3_set_ass[setassoc_i][i].valid_bit==1)
									{
										num_elements++;
									}
								}
								remaining_space=L3_numlines-num_elements;
				
								while (counter>0)
								{
									if (holder->tag_bits==tag_index)
									{
										cold_flag=1;
										break;
									}
					
									else
									{
										searchi++;
						
										if (&L3_set_ass[setassoc_i][searchi]!=NULL)
										{
											holder=&L3_set_ass[setassoc_i][searchi];
										}
									}
					
									counter--;
								}
				
								if (cold_flag==1)
								{
									L3_total_hits++;
					
									if (policy_flag==2)
									{
										setLRUhit(tag_index, 3,setassoc_i);
									}
								}
				
								else if (cold_flag==0)
								{
									L3_total_misses++;
									
									if(remaining_space!=0)
									{
										searchi=0;
										holder=&L3_set_ass[setassoc_i][0];
										while (holder->valid_bit!=0)
										{
											searchi++;
					
											if (&L3_set_ass[setassoc_i][searchi]!=NULL)
											{
												holder=&L3_set_ass[setassoc_i][searchi];
											}
										}
										
										holder->valid_bit=1;
										holder->tag_bits= tag_index;
										L3_cold_misses++;
										setenq(holder,setassoc_i,3); 
									}
									else if(remaining_space==0)
									{
										long long tag_replace=0;
										
										tag_replace=setdequeue(3,setassoc_i);
										
										LINE *guide=&L3_set_ass[setassoc_i][0];
										int a_index=0;
										
										while (guide->tag_bits!=tag_replace)
										{
											a_index++;
											
											if (&L3_set_ass[setassoc_i][a_index]!=NULL)
											{
												guide=&L3_set_ass[setassoc_i][a_index];
											}
										}
										
										guide->tag_bits=tag_index;
										
										setenq(guide, setassoc_i,3);
									}
									
								}
							}
						}
					}
					else if (leng2>5 && (strcmp(argv[8],direct)!=0)) /* set associative */
					{
						temp_hold_hit=L2_total_hits;
				
						temp_hold=L2_total_misses;
				
						temp_cold_hold=L2_cold_misses;
				
						counter= L2_numlines;
				
						searchi=0;
				
						num_elements=0;
				
						fullsize=(two_size)/(block_size);
				
						cold_flag=0;
				
						setassoc_mask= L2_sets_test-1;
				
						setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
						tagbitshift= L2_s_bits+b_bits;
				
						tag_sumsize=pow(2,L2_tag_bits);
				
						tag_mask=tag_sumsize-1;
				
						tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
						holder= &L2_set_ass[setassoc_i][0];
				
						for (int i=0; i<L2_numlines;i++)
						{
							if (L2_set_ass[setassoc_i][i].valid_bit==1)
							{
								num_elements++;
							}
						}
						
						remaining_space=L2_numlines-num_elements;
				
						while (counter>0)
						{
							if (holder->tag_bits==tag_index)
							{
								cold_flag=1;
								break;
							}
					
							else
							{
								searchi++;
						
								if (&L2_set_ass[setassoc_i][searchi]!=NULL)
								{
									holder=&L2_set_ass[setassoc_i][searchi];
								}
							}
					
							counter--;
						}
				
						if (cold_flag==1)
						{
							L2_total_hits++;
					
							if (policy_flag==2)
							{
								setLRUhit(tag_index, 2,setassoc_i);
							}
						}
				
						else if (cold_flag==0)
						{
							L2_total_misses++;
									
							if(remaining_space!=0)
							{
								searchi=0;
								holder=&L2_set_ass[setassoc_i][0];
								
								while (holder->valid_bit!=0)
								{
									searchi++;
					
									if (&L2_set_ass[setassoc_i][searchi]!=NULL)
									{
										holder=&L2_set_ass[setassoc_i][searchi];
									}
								}
										
								holder->valid_bit=1;
								holder->tag_bits= tag_index;
								L2_cold_misses++;
								setenq(holder,setassoc_i,2); 
							}
							
							else if(remaining_space==0)
							{
								long long tag_replace=0;
										
								tag_replace=setdequeue(2,setassoc_i);
										
								LINE *guide=&L2_set_ass[setassoc_i][0];
								int a_index=0;
										
								while (guide->tag_bits!=tag_replace)
								{
									a_index++;
											
									if (&L2_set_ass[setassoc_i][a_index]!=NULL)
									{
										guide=&L2_set_ass[setassoc_i][a_index];
									}
								}
										
								guide->tag_bits=tag_index;
										
								setenq(guide, setassoc_i,2);
							}
									
						}
						
						if (L2_total_misses>temp_hold) /* go to third cache */
						{
							if (strcmp(argv[12],direct)==0)
							{
								function_result=directmap(hex_value, L3_set_list, L3_s_bits, b_bits, L3_tag_bits, L3_total_misses, L3_total_hits, L3_cold_misses);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
								else
								{
									L3_total_misses++;
								}
							}
							else if (strcmp(argv[12],assoc)==0) /* full associative*/
							{
								function_result=fullassoc(hex_value, L3_line_list, b_bits, L3_numlines, L3_cold_misses,L3_capacity_misses, L3_total_misses, 3, L3_tag_bits,L3_total_hits, policy_flag);
							
								if(function_result==1)
								{
									L3_cold_misses++;
								
								}
				
								if(function_result==3)
								{
									L3_total_hits++;
								}
							
								else
								{	
									L3_total_misses++;
								}
							}
							else if (leng3>5 && (strcmp(argv[12],direct)!=0)) /* set associative */
							{
								counter= L3_numlines;
				
								searchi=0;
				
								num_elements=0;
				
								fullsize=(three_size)/(block_size);
				
								cold_flag=0;
				
								setassoc_mask= L3_sets_test-1;
				
								setassoc_i=(hex_value >> b_bits) & setassoc_mask;
				
								tagbitshift= L3_s_bits+b_bits;
				
								tag_sumsize=pow(2,L3_tag_bits);
				
								tag_mask=tag_sumsize-1;
				
								tag_index= (hex_value >> tagbitshift) & tag_mask; 
				
								holder= &L3_set_ass[setassoc_i][0];
				
								for (int i=0; i<L3_numlines;i++)
								{
									if (L3_set_ass[setassoc_i][i].valid_bit==1)
									{
										num_elements++;
									}
								}
								remaining_space=L3_numlines-num_elements;
				
								while (counter>0)
								{
									if (holder->tag_bits==tag_index)
									{
										cold_flag=1;
										break;
									}
					
									else
									{
										searchi++;
						
										if (&L3_set_ass[setassoc_i][searchi]!=NULL)
										{
											holder=&L3_set_ass[setassoc_i][searchi];
										}
									}
					
									counter--;
								}
				
								if (cold_flag==1)
								{
									L3_total_hits++;
					
									if (policy_flag==2)
									{
										setLRUhit(tag_index, 3,setassoc_i);
									}
								}
			
								else if (cold_flag==0)
								{
									L3_total_misses++;
									
									if(remaining_space!=0)
									{
										searchi=0;
										holder=&L3_set_ass[setassoc_i][0];
										while (holder->valid_bit!=0)
										{
											searchi++;
					
											if (&L3_set_ass[setassoc_i][searchi]!=NULL)
											{
												holder=&L3_set_ass[setassoc_i][searchi];
											}
										}
										
										holder->valid_bit=1;
										holder->tag_bits= tag_index;
										L3_cold_misses++;
										setenq(holder,setassoc_i,3); 
									}
									else if(remaining_space==0)
									{
										long long tag_replace=0;
										
										tag_replace=setdequeue(3,setassoc_i);
										
										LINE *guide=&L3_set_ass[setassoc_i][0];
										int a_index=0;
										
										while (guide->tag_bits!=tag_replace)
										{
											a_index++;
											
											if (&L3_set_ass[setassoc_i][a_index]!=NULL)
											{
												guide=&L3_set_ass[setassoc_i][a_index];
											}
										}
										
										guide->tag_bits=tag_index;
										
										setenq(guide, setassoc_i,3);
									}
								}
							}
						}	
					}
				}
			}
		}
	}
	
	fclose(fp);
	
	long long sim_capacity= 0;
	long long total_conflict= 0;
	
	if(strcmp(argv[4],direct)==0)
	{
		sim_capacity= L1_sim_misses-L1_sim_cold;
		total_conflict= L1_total_misses-(L1_cold_misses)-(sim_capacity);
		capacity_misses=sim_capacity;
	}
	
	if(leng>5 && (strcmp(argv[4],direct)!=0))
	{
		sim_capacity= L1_sim_misses-L1_sim_cold;
		total_conflict= L1_total_misses-(L1_cold_misses)-(sim_capacity);
		capacity_misses=sim_capacity;
	}
	if (strcmp(argv[4],assoc)==0)
	{
		capacity_misses= L1_total_misses-(L1_cold_misses);
		total_conflict=0;
	}
	
	printf("Memory accesses: %d\n", memory_accesses);
	printf("L1 Cache hits: %lld\n", L1_total_hits);
	printf("L1 Cache miss: %lld\n", L1_total_misses);
	printf("L2 Cache hits: %lld\n", L2_total_hits);
	printf("L2 Cache miss: %lld\n", L2_total_misses);
	printf("L3 Cache hits: %lld\n", L3_total_hits);
	printf("L3 Cache miss: %lld\n", L3_total_misses);
	printf("L1 Cold misses: %lld\n",L1_cold_misses);
	printf("L2 Cold misses: %lld\n",L2_cold_misses);
	printf("L3 Cold misses: %lld\n",L3_cold_misses);
	printf("L1 Conflict misses: %lld\n", total_conflict);
	printf("L1 Capacity misses: %lld\n",capacity_misses);
	
	LINE *freedom;
	
	while (L1_q_head!=NULL)
	{
		freedom=L1_q_head;
		L1_q_head=L1_q_head->next;
		free(freedom);
	}
	
	while (L2_q_head!=NULL)
	{
		freedom=L2_q_head;
		L2_q_head=L2_q_head->next;
		free(freedom);
	}
	
	while (L3_q_head!=NULL)
	{
		freedom=L3_q_head;
		L3_q_head=L3_q_head->next;
		free(freedom);
	}
	
	while (SIM_q_head!=NULL)
	{
		freedom=SIM_q_head;
		SIM_q_head=SIM_q_head->next;
		free(freedom);
	}
	
	free(temp);
	
	return 0;
}

int fullassoc(unsigned long long hex, LINE list[], long long bbits, long long num_lines, long long cold_misses,long long capacity_misses, long long total_misses, int q_index, long long tagbits, long long total_hits, int policy)
{
	
	long long num_elements=0;
	
	long long tag_size= pow(2,tagbits);
	long long tag_mask=tag_size-1;
	
	int searchi=0;
	
	long long counter=num_lines;
	
	LINE *temp=&list[0];
	
	while (counter>0)
	{
		if (temp->valid_bit==1)
		{
			num_elements++;
		}
		
		searchi++;
		
		if (&list[searchi]!=NULL)
		{
			temp=&list[searchi];
		}
		
		counter--;
	}
	
	long long remain_space= num_lines-num_elements;
		
	long long tag_index= (hex >> bbits) & tag_mask;
	
	int index=0;
	
	long long count=num_lines;
	
	LINE *ptr= &list[0];
	
	int cold_flag=0;
	
	while (count>0)
	{
		if (ptr->tag_bits==tag_index)
		{
				cold_flag=1;
				break;
		}
		
		else
		{
				index++;
		
				if (&list[index]!=NULL)
				{
					ptr=&list[index];
				}
		}
		
		count--;
	}
	
	if (cold_flag==1) /* tag_index was found */
	{
		if(ptr->valid_bit==1)
		{
			
			
			if (policy==2)
			{	/* LRU Hit policy: Move the hit item to the back of the list to indicate
				 * most recently used.*/
				 
				 LRUhit(tag_index,q_index);
			}
			return 3;
		}
	}
	
	else if (cold_flag==0)
	{
		if (remain_space!=0) /* available empty lines for a cold miss*/
		{
			int finder=0;
			
			LINE *find= &list[0];
			
			while (find->valid_bit!=0)
			{
				finder++;
				
				if (&list[finder]!=NULL)
				{
					find=&list[finder];
				}
			}
			
			
			find->valid_bit=1;
			find->tag_bits= tag_index;
			
			enqueue(find,q_index); 
			return 1;	
		}
		
		else if (remain_space==0)
		{
			
			long long tag_replace=0;
			
			tag_replace= dequeue(q_index);
			
			LINE *guide= &list[0];
			int a_index=0;
			
			while (guide->tag_bits!=tag_replace)
			{
				a_index++;
				
				if (&list[a_index]!=NULL)
				{
					guide=&list[a_index];
				}
			}
			
			guide->tag_bits= tag_index;
				
			enqueue(guide, q_index);	
		}
	}
	
	return 0;
}

int directmap(unsigned long long hex, LINE list[], long long sbits, int bbits, long long tagbits, long long total_misses, long long total_hits, long long cold_misses)
{
	int output_flag=0;
	
	long long tag_sum= bbits+sbits;
	
	long long set_mask_size=pow(2,sbits);
	
	long long set_mask= set_mask_size-1;
	
	long long set_i= (hex >> bbits) & set_mask;
	
	long long tag_size= pow(2, tagbits);
	
	long long tag_mask= tag_size-1;
	
	long long hex_tag= (hex >> tag_sum) & tag_mask;
	
	
	
	LINE *head= &list[set_i];
	
	if (head->valid_bit==0)
	{
		output_flag=1;
		head->valid_bit=1;
		head->tag_bits= hex_tag;
		head->next=NULL;
	}
	
	else if (head->valid_bit==1)
	{
		if (head->tag_bits == hex_tag)
		{
			output_flag=3;
		}
		else
		{
			head->tag_bits= hex_tag;
		}
	}
	return output_flag;
}
void enqueue(LINE *item, int q_i)
{
	LINE *temp=(LINE*)malloc(sizeof(LINE));
	temp->valid_bit= item->valid_bit;
	temp->tag_bits= item->tag_bits;
	temp->next=NULL;
	
	if (q_i==1) /* L1 Cache*/
	{
	
		if(L1_q_head==NULL)
		{
			L1_q_head=(LINE*)malloc(sizeof(LINE));
			L1_q_head->valid_bit= item->valid_bit;
			L1_q_head->tag_bits= item->tag_bits;
			free(temp);
			L1_q_head->next=NULL;
		}
	
		else if (L1_q_head!=NULL)
		{
			LINE *ptr=L1_q_head;
			LINE *prev=L1_q_head;
		
			while (ptr!=NULL)
			{
				prev=ptr;
				ptr=ptr->next;
			}
		
			prev->next=temp;
		}
	}
	
	if (q_i==2) /* L2 Cache*/ 
	{
		if(L2_q_head==NULL)
		{
			L2_q_head=(LINE*)malloc(sizeof(LINE));
			L2_q_head->valid_bit= item->valid_bit;
			L2_q_head->tag_bits= item->tag_bits;
			free(temp);
			L2_q_head->next=NULL;
		}
	
		else if (L2_q_head!=NULL)
		{
			LINE *ptr=L2_q_head;
			LINE *prev=L2_q_head;
		
			while (ptr!=NULL)
			{
				prev=ptr;
				ptr=ptr->next;
			}
		
			prev->next=temp;
		}
	}
	
	if (q_i==3) /*L3 Cache */
	{
		if(L3_q_head==NULL)
		{
			L3_q_head=(LINE*)malloc(sizeof(LINE));
			L3_q_head->valid_bit= item->valid_bit;
			L3_q_head->tag_bits= item->tag_bits;
			free(temp);
			L3_q_head->next=NULL;
		}
	
		else if (L3_q_head!=NULL)
		{
			LINE *ptr=L3_q_head;
			LINE *prev=L3_q_head;
		
			while (ptr!=NULL)
			{
				prev=ptr;
				ptr=ptr->next;
			}
		
			prev->next=temp;
		}
	}
	
	if (q_i==4) /* SIM QUEUE*/
	{
		if(SIM_q_head==NULL)
		{
			SIM_q_head=(LINE*)malloc(sizeof(LINE));
			SIM_q_head->valid_bit= item->valid_bit;
			SIM_q_head->tag_bits= item->tag_bits;
			free(temp);
			SIM_q_head->next=NULL;
		}
	
		else if (SIM_q_head!=NULL)
		{
			LINE *ptr=SIM_q_head;
			LINE *prev=SIM_q_head;
		
			while (ptr!=NULL)
			{
				prev=ptr;
				ptr=ptr->next;
			}
		
			prev->next=temp;
		}
	}
}

long long dequeue(int q_i)
{
	long long tag_return=0;
	if (q_i==1)
	{
		
		if (L1_q_head->next==NULL)
		{
			tag_return= L1_q_head->tag_bits;
			L1_q_head=NULL;
			free(L1_q_head);
			return tag_return;
		}
	
		else if(L1_q_head->next!=NULL)
		{
			tag_return= L1_q_head->tag_bits;
			LINE *fill=L1_q_head;
			L1_q_head=L1_q_head->next;
			free(fill);
		}
	}
	
	if (q_i==2)
	{
		if (L2_q_head->next==NULL)
		{
			tag_return= L2_q_head->tag_bits;
			L2_q_head=NULL;
			free(L2_q_head);
			return tag_return;
		}
	
		else if(L2_q_head->next!=NULL)
		{
			tag_return= L2_q_head->tag_bits;
			LINE *fill=L2_q_head;
			L2_q_head=L2_q_head->next;
			free(fill);
		}
	}
	if (q_i==3)
	{
		if (L3_q_head->next==NULL)
		{
			tag_return= L3_q_head->tag_bits;
			L3_q_head=NULL;
			free(L3_q_head);
			return tag_return;
		}
	
		else if(L3_q_head->next!=NULL)
		{
			tag_return= L3_q_head->tag_bits;
			LINE *fill=L3_q_head;
			L3_q_head=L3_q_head->next;
			free(fill);
		}
	}
	
	if (q_i==4)
	{
		if (SIM_q_head->next==NULL)
		{
			tag_return= SIM_q_head->tag_bits;
			SIM_q_head=NULL;
			free(SIM_q_head);
			return tag_return;
		}
	
		else if(SIM_q_head->next!=NULL)
		{
			tag_return= SIM_q_head->tag_bits;
			LINE *fill=SIM_q_head;
			SIM_q_head=SIM_q_head->next;
			free(fill);
		}
	}
	
	return tag_return;
}

void LRUhit(long long tagindex, int q_i)
{
	if (q_i==1)
	{
		LINE *ptr= L1_q_head;
		LINE *prev_fill= L1_q_head;
	
		while(ptr!=NULL)
		{
			prev_fill=ptr;
			ptr=ptr->next;
		}
	
		if (L1_q_head->tag_bits== tagindex)
		{
			LINE *temp=(LINE*)malloc(sizeof(LINE));
	 
			temp->valid_bit= L1_q_head->valid_bit;
			temp->tag_bits= L1_q_head->tag_bits;
			temp->next=NULL;
	 
			prev_fill->next= temp;
			LINE *hold= L1_q_head;
			L1_q_head=L1_q_head->next;
			free(hold);
		}
	
		else
		{
			ptr= L1_q_head;
			LINE *prev=L1_q_head;
		
			while (ptr->tag_bits!=tagindex)
			{
				prev=ptr;
				ptr=ptr->next;
			}
		
			LINE *holder= (LINE*)malloc(sizeof(LINE));
			holder->valid_bit= ptr->valid_bit;
			holder->tag_bits= ptr->tag_bits;
			holder->next=NULL;
		
			prev_fill->next= holder;
			LINE *hold= ptr;
			prev->next=ptr->next;
			free(hold);
		}
	}
		
	if (q_i==2)
	{
		LINE *ptr= L2_q_head;
		LINE *prev_fill= L2_q_head;
	
		while(ptr!=NULL)
		{
			prev_fill=ptr;
			ptr=ptr->next;
		}
	
		if (L2_q_head->tag_bits== tagindex)
		{
			LINE *temp=(LINE*)malloc(sizeof(LINE));
	 
			temp->valid_bit= L2_q_head->valid_bit;
			temp->tag_bits= L2_q_head->tag_bits;
			temp->next=NULL;
	 
			prev_fill->next= temp;
			LINE *hold= L2_q_head;
			L2_q_head=L2_q_head->next;
			free(hold);
		}
	
		else
		{
			ptr= L2_q_head;
			LINE *prev=L2_q_head;
		
			while (ptr->tag_bits!=tagindex)
			{
				prev=ptr;
				ptr=ptr->next;
			}
		
			LINE *holder= (LINE*)malloc(sizeof(LINE));
			holder->valid_bit= ptr->valid_bit;
			holder->tag_bits= ptr->tag_bits;
			holder->next=NULL;
		
			prev_fill->next= holder;
			LINE *hold= ptr;
			prev->next=ptr->next;
			free(hold);
		}
	}
		
	if (q_i==3)
	{
		LINE *ptr= L3_q_head;
		LINE *prev_fill= L3_q_head;
	
		while(ptr!=NULL)
		{
			prev_fill=ptr;
			ptr=ptr->next;
		}
	
		if (L3_q_head->tag_bits== tagindex)
		{
			LINE *temp=(LINE*)malloc(sizeof(LINE));
	 
			temp->valid_bit= L3_q_head->valid_bit;
			temp->tag_bits= L3_q_head->tag_bits;
			temp->next=NULL;
	 
			prev_fill->next= temp;
			LINE *hold= L3_q_head;
			L3_q_head=L3_q_head->next;
			free(hold);
		}
	
		else
		{
			ptr= L3_q_head;
			LINE *prev=L3_q_head;
		
			while (ptr->tag_bits!=tagindex)
			{
				prev=ptr;
				ptr=ptr->next;
			}
		
			LINE *holder= (LINE*)malloc(sizeof(LINE));
			holder->valid_bit= ptr->valid_bit;
			holder->tag_bits= ptr->tag_bits;
			holder->next=NULL;
		
			prev_fill->next= holder;
			LINE *hold= ptr;
			prev->next=ptr->next;
			free(hold);
		}
	}
	
	if (q_i==4)
	{
		LINE *ptr= SIM_q_head;
		LINE *prev_fill= SIM_q_head;
	
		while(ptr!=NULL)
		{
			prev_fill=ptr;
			ptr=ptr->next;
		}
	
		if (SIM_q_head->tag_bits== tagindex)
		{
			LINE *temp=(LINE*)malloc(sizeof(LINE));
	 
			temp->valid_bit= SIM_q_head->valid_bit;
			temp->tag_bits= SIM_q_head->tag_bits;
			temp->next=NULL;
	 
			prev_fill->next= temp;
			LINE *hold= SIM_q_head;
			SIM_q_head=SIM_q_head->next;
			free(hold);
		}
	
		else
		{
			ptr= SIM_q_head;
			LINE *prev=SIM_q_head;
		
			while (ptr->tag_bits!=tagindex)
			{
				prev=ptr;
				ptr=ptr->next;
			}
		
			LINE *holder= (LINE*)malloc(sizeof(LINE));
			holder->valid_bit= ptr->valid_bit;
			holder->tag_bits= ptr->tag_bits;
			holder->next=NULL;
		
			prev_fill->next= holder;
			LINE *hold= ptr;
			prev->next=ptr->next;
			free(hold);
		}
	}
}
void delete (LINE *q_head)
{
	LINE *freedom;
	
	while (q_head!=NULL)
	{
		freedom=q_head;
		q_head=q_head->next;
		free(freedom);
	}
}
void setenq(LINE *item, int s_index, int q_i)
{
	LINE *temp=(LINE*)malloc(sizeof(LINE));
	temp->valid_bit= item->valid_bit;
	temp->tag_bits= item->tag_bits;
	temp->search_index=s_index;
	temp->next=NULL;
	
	if (q_i==1) /* L1 Cache*/
	{
	
		if(L1_q_head==NULL)
		{
			L1_q_head=(LINE*)malloc(sizeof(LINE));
			L1_q_head->valid_bit= item->valid_bit;
			L1_q_head->search_index=s_index;
			L1_q_head->tag_bits= item->tag_bits;
			free(temp);
			L1_q_head->next=NULL;
		}
	
		else if (L1_q_head!=NULL)
		{
			LINE *ptr=L1_q_head;
			LINE *prev=L1_q_head;
			int count=0;
		
			while (ptr!=NULL)
			{
				if(ptr->search_index==s_index)
				{
					count++;
				}
				prev=ptr;
				ptr=ptr->next;
			}
			
			if(count==0)
			{
				prev->next=temp;
				return;
			}
			
			ptr=L1_q_head;
			prev=L1_q_head;
			
			while(count>0)
			{
				if(ptr->search_index==s_index)
				{
					count--;
				}
				prev=ptr;
				ptr=ptr->next;
			}
			
			temp->next=prev->next;
			prev->next=temp;
		}
	}
	
	if (q_i==2) /* L2 Cache*/ 
	{
		if(L2_q_head==NULL)
		{
			L2_q_head=(LINE*)malloc(sizeof(LINE));
			L2_q_head->valid_bit= item->valid_bit;
			L2_q_head->search_index=s_index;
			L2_q_head->tag_bits= item->tag_bits;
			free(temp);
			L2_q_head->next=NULL;
		}
	
		else if (L2_q_head!=NULL)
		{
			LINE *ptr=L2_q_head;
			LINE *prev=L2_q_head;
		
			while (ptr!=NULL)
			{
				prev=ptr;
				ptr=ptr->next;
			}
		
			prev->next=temp;
		}
	}
	
	if (q_i==3) /*L3 Cache */
	{
		if(L3_q_head==NULL)
		{
			L3_q_head=(LINE*)malloc(sizeof(LINE));
			L3_q_head->valid_bit= item->valid_bit;
			L3_q_head->search_index=s_index;
			L3_q_head->tag_bits= item->tag_bits;
			free(temp);
			L3_q_head->next=NULL;
		}
	
		else if (L3_q_head!=NULL)
		{
			LINE *ptr=L3_q_head;
			LINE *prev=L3_q_head;
		
			while (ptr!=NULL)
			{
				prev=ptr;
				ptr=ptr->next;
			}
		
			prev->next=temp;
		}
	}
	
	if (q_i==4) /* SIM QUEUE*/
	{
		if(SIM_q_head==NULL)
		{
			SIM_q_head=(LINE*)malloc(sizeof(LINE));
			SIM_q_head->valid_bit= item->valid_bit;
			SIM_q_head->search_index=s_index;
			SIM_q_head->tag_bits= item->tag_bits;
			free(temp);
			SIM_q_head->next=NULL;
		}
	
		else if (SIM_q_head!=NULL)
		{
			LINE *ptr=SIM_q_head;
			LINE *prev=SIM_q_head;
		
			while (ptr!=NULL)
			{
				prev=ptr;
				ptr=ptr->next;
			}
		
			prev->next=temp;
		}
	}
}
long long setdequeue(int q_i, int search_i)
{
	long long tag_return=0;
	
	if (q_i==1)
	{
		if (L1_q_head->next==NULL && L1_q_head->search_index==search_i)
		{
			tag_return= L1_q_head->tag_bits;
			L1_q_head=NULL;
			free(L1_q_head);
			return tag_return;
		}
	
		else if(L1_q_head->next!=NULL)
		{
			if(L1_q_head->search_index==search_i)
			{
				tag_return=L1_q_head->tag_bits;
				LINE *fill=L1_q_head;
				L1_q_head=L1_q_head->next;
				free(fill);
			}
			else
			{
				LINE *ptr= L1_q_head;
				LINE *prev=L1_q_head;
			
				while (ptr->search_index!=search_i)
				{
					prev=ptr;
					ptr=ptr->next;
				}
				
				tag_return= ptr->tag_bits;
				prev->next=ptr->next;
				LINE *fill=ptr;
				
				free(fill);
			}
		}
	}
	
	if (q_i==2)
	{
		if (L2_q_head->next==NULL && L2_q_head->search_index==search_i)
		{
			tag_return= L2_q_head->tag_bits;
			L2_q_head=NULL;
			free(L2_q_head);
			return tag_return;
		}
	
		else if(L2_q_head->next!=NULL)
		{
			if(L2_q_head->search_index==search_i)
			{
				tag_return=L2_q_head->tag_bits;
				LINE *fill=L2_q_head;
				L2_q_head=L2_q_head->next;
				free(fill);
			}
			else
			{
				LINE *ptr= L2_q_head;
				LINE *prev=L2_q_head;
			
				while (ptr->search_index!=search_i)
				{
					prev=ptr;
					ptr=ptr->next;
				}
				
				tag_return= ptr->tag_bits;
				prev->next=ptr->next;
				LINE *fill=ptr;
				ptr->next=prev->next;
				free(fill);
			}
		}
	}
	if (q_i==3)
	{
		if (L3_q_head->next==NULL && L3_q_head->search_index==search_i)
		{
			tag_return= L3_q_head->tag_bits;
			L3_q_head=NULL;
			free(L3_q_head);
			return tag_return;
		}
	
		else if(L3_q_head->next!=NULL)
		{
			if(L3_q_head->search_index==search_i)
			{
				tag_return=L3_q_head->tag_bits;
				LINE *fill=L3_q_head;
				L3_q_head=L3_q_head->next;
				free(fill);
			}
			else
			{
				LINE *ptr= L3_q_head;
				LINE *prev=L3_q_head;
			
				while (ptr->search_index!=search_i)
				{
					prev=ptr;
					ptr=ptr->next;
				}
				
				tag_return= ptr->tag_bits;
				prev->next=ptr->next;
				LINE *fill=ptr;
				ptr->next=prev->next;
				free(fill);
			}
		}
	}
	
	if (q_i==4)
	{
		if (SIM_q_head->next==NULL && SIM_q_head->search_index==search_i)
		{
			tag_return= SIM_q_head->tag_bits;
			SIM_q_head=NULL;
			free(SIM_q_head);
			return tag_return;
		}
	
		else if(SIM_q_head->next!=NULL)
		{
			if(SIM_q_head->search_index==search_i)
			{
				tag_return=SIM_q_head->tag_bits;
				LINE *fill=SIM_q_head;
				SIM_q_head=SIM_q_head->next;
				free(fill);
			}
			else
			{
				LINE *ptr= SIM_q_head;
				LINE *prev=SIM_q_head;
			
				while (ptr->search_index!=search_i)
				{
					prev=ptr;
					ptr=ptr->next;
				}
				
				tag_return= ptr->tag_bits;
				prev->next=ptr->next;
				LINE *fill=ptr;
				ptr->next=prev->next;
				free(fill);
			}
		}
	}
	
	return tag_return;
}
void setLRUhit(long long tagindex, int q_i, int setindex)
{
	int s_index=0;
	int count=0;
	
	if (q_i==1)
	{
		LINE *ptr= L1_q_head;
		
		s_index=setindex;
		
		while (ptr!=NULL)
		{
			if(ptr->search_index==s_index)
			{
				count++;
			}
			ptr=ptr->next;
		}
		
		if(count!=1)
		{
			ptr=L1_q_head;
			LINE *prev=L1_q_head;
		
			while (count>0)
			{
				if(ptr->search_index==s_index)
				{
					count--;
				}
				prev=ptr;
				ptr=ptr->next;
			}
			
			if (prev->tag_bits==tagindex)
			{
				return;
			}
			
			if(L1_q_head->next==NULL)
			{
				return;
			}

			ptr= L1_q_head;
			LINE *prev_fill=L1_q_head;
		
			while (ptr!=NULL)
			{
				if (ptr->search_index==setindex)
				{
					if (ptr->tag_bits==tagindex)
					{
						break;
					}
				}
					
				prev_fill=ptr;
				ptr=ptr->next;
			}
					
			LINE *holder= (LINE*)malloc(sizeof(LINE));
			holder->valid_bit= ptr->valid_bit;
			holder->search_index=ptr->search_index;
			holder->tag_bits= ptr->tag_bits;
			
			if(ptr==L1_q_head)
			{
				holder->next=prev->next;
				prev->next=holder;
				LINE *hold= L1_q_head;
				L1_q_head=L1_q_head->next;
				free(hold);
			}	
			else
			{
				holder->next=prev->next;
				prev->next= holder;
				
				LINE *hold= ptr;
				prev_fill->next=ptr->next;
				free(hold);
			}
		}
	}
		
	if (q_i==2)
	{
		LINE *ptr= L2_q_head;
		
		s_index=setindex;
		
		while (ptr!=NULL)
		{
			if(ptr->search_index==s_index)
			{
				count++;
			}
			ptr=ptr->next;
		}
		
		if(count!=1)
		{
			ptr=L2_q_head;
			LINE *prev=L2_q_head;
		
			while (count>0)
			{
				if(ptr->search_index==s_index)
				{
					count--;
				}
				prev=ptr;
				ptr=ptr->next;
			}
			
			if (prev->tag_bits==tagindex)
			{
				return;
			}
			
			if(L2_q_head->next==NULL)
			{
				return;
			}

			ptr= L2_q_head;
			LINE *prev_fill=L2_q_head;
		
			while (ptr!=NULL)
			{
				if (ptr->search_index==setindex)
				{
					if (ptr->tag_bits==tagindex)
					{
						break;
					}
				}
					
				prev_fill=ptr;
				ptr=ptr->next;
			}
					
			LINE *holder= (LINE*)malloc(sizeof(LINE));
			holder->valid_bit= ptr->valid_bit;
			holder->search_index=ptr->search_index;
			holder->tag_bits= ptr->tag_bits;
			
			if(ptr==L2_q_head)
			{
				holder->next=prev->next;
				prev->next=holder;
				LINE *hold= L2_q_head;
				L2_q_head=L2_q_head->next;
				free(hold);
			}	
			else
			{
				holder->next=prev->next;
				prev->next= holder;
				
				LINE *hold= ptr;
				prev_fill->next=ptr->next;
				free(hold);
			}
		}
	}
		
	if (q_i==3)
	{
		LINE *ptr= L3_q_head;
		
		s_index=setindex;
		
		while (ptr!=NULL)
		{
			if(ptr->search_index==s_index)
			{
				count++;
			}
			ptr=ptr->next;
		}
		
		if(count!=1)
		{
			ptr=L3_q_head;
			LINE *prev=L3_q_head;
		
			while (count>0)
			{
				if(ptr->search_index==s_index)
				{
					count--;
				}
				prev=ptr;
				ptr=ptr->next;
			}
			
			if (prev->tag_bits==tagindex)
			{
				return;
			}
			
			if(L3_q_head->next==NULL)
			{
				return;
			}

			ptr= L3_q_head;
			LINE *prev_fill=L3_q_head;
		
			while (ptr!=NULL)
			{
				if (ptr->search_index==setindex)
				{
					if (ptr->tag_bits==tagindex)
					{
						break;
					}
				}
					
				prev_fill=ptr;
				ptr=ptr->next;
			}
					
			LINE *holder= (LINE*)malloc(sizeof(LINE));
			holder->valid_bit= ptr->valid_bit;
			holder->search_index=ptr->search_index;
			holder->tag_bits= ptr->tag_bits;
			
			if(ptr==L3_q_head)
			{
				holder->next=prev->next;
				prev->next=holder;
				LINE *hold= L3_q_head;
				L3_q_head=L3_q_head->next;
				free(hold);
			}	
			else
			{
				holder->next=prev->next;
				prev->next= holder;
				
				LINE *hold= ptr;
				prev_fill->next=ptr->next;
				free(hold);
			}
		}
	}
	
	if (q_i==4)
	{
		LINE *ptr= SIM_q_head;
		
		s_index=setindex;
		
		while (ptr!=NULL)
		{
			if(ptr->search_index==s_index)
			{
				count++;
			}
			ptr=ptr->next;
		}
		
		if(count!=1)
		{
			ptr=SIM_q_head;
			LINE *prev=SIM_q_head;
		
			while (count>0)
			{
				if(ptr->search_index==s_index)
				{
					count--;
				}
				prev=ptr;
				ptr=ptr->next;
			}
			
			if (prev->tag_bits==tagindex)
			{
				return;
			}
			
			if(SIM_q_head->next==NULL)
			{
				return;
			}

			ptr= SIM_q_head;
			LINE *prev_fill=SIM_q_head;
		
			while (ptr!=NULL)
			{
				if (ptr->search_index==setindex)
				{
					if (ptr->tag_bits==tagindex)
					{
						break;
					}
				}
					
				prev_fill=ptr;
				ptr=ptr->next;
			}
					
			LINE *holder= (LINE*)malloc(sizeof(LINE));
			holder->valid_bit= ptr->valid_bit;
			holder->search_index=ptr->search_index;
			holder->tag_bits= ptr->tag_bits;
			
			if(ptr==SIM_q_head)
			{
				holder->next=prev->next;
				prev->next=holder;
				LINE *hold= SIM_q_head;
				SIM_q_head=SIM_q_head->next;
				free(hold);
			}	
			else
			{
				holder->next=prev->next;
				prev->next= holder;
				
				LINE *hold= ptr;
				prev_fill->next=ptr->next;
				free(hold);
			}
		}
	}
}

