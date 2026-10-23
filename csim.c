/*
 * @Author: zero-extend 3312154119@qq.com
 * @Date: 2026-09-21 18:26:58
 * @LastEditors: zero-extend 3312154119@qq.com
 * @LastEditTime: 2026-10-23 22:20:46
 * @FilePath: /cachelab-handout/csim.c
 * @Description: 
 * 
 * Copyright (c) 2026 by ${zero-extend}, All Rights Reserved. 
 */


#include "cachelab.h"
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>



/** s: get s bit for address to calculate set index .need 2^s set*/
int s = 0 , E = 0 , b = 0;
/** time of now */
int time = 0;
/** count of Set */
int Set_count = 0; 


/** add count */
int hit_count = 0;
int miss_count = 0;
int change_count = 0;

typedef struct cache_line
{
    unsigned long Tag;
    int valid;
    int LRU_time;
}cache_line;



cache_line** pointer = NULL;

void cache_init(int Set_count,int E);
void access_cache(unsigned long address);


void cache_init(int Set_count,int E){

    /**  1.malloc place for cache Set-arr */
    pointer = malloc(sizeof(cache_line*)*Set_count);
    
    for (int i = 0; i < Set_count; i++)
    {
        /** 2.malloc place for cache line */      
        pointer[i] = malloc(sizeof(cache_line)*E);
    }
    
}

void access_cache(unsigned long address){
    unsigned long a1,a2;
    unsigned long mask;
    a1 = address >> (s + b);   // get the part1(Tag)
    mask = (1 << s)-1;
    a2 = (address >> b) & mask;  // get the part2(Set index)
    

    /** define flag ,be clean as 0 every get in*/
    int Tag_flag = 0;
    int Efree_flag = 0;
    int Efree_index = 0;
    /** min_LRu */
    int min_LRUindex = 0;

    time++;
    
    for (int i = 0; i < E; i++)
    {
        if ((pointer[a2][i].Tag == a1) && (pointer[a2][i].valid == 1))
        {
            /** cache hit */
            hit_count++;
            Tag_flag = 1;
            pointer[a2][i].LRU_time = time;
            return ;
        }

        /** log the free En */
        if (pointer[a2][i].valid == 0)
        {
            Efree_flag = 1;
            Efree_index = i;
        }

        /** find the smallest LRU index */
        if (pointer[a2][i].LRU_time < pointer[a2][min_LRUindex].LRU_time)
        {
            min_LRUindex = i;
        }
        
        
    }

    /** cache miss */
    if (Tag_flag == 0)
    {   
        miss_count++;

        /** have free En */
        if (Efree_flag == 1)
        {
            pointer[a2][Efree_index].LRU_time = time;
            pointer[a2][Efree_index].Tag = a1;
            pointer[a2][Efree_index].valid = 1;
            Efree_flag = 0;
        }
        
        /** don't have free En */
        else{
            change_count++;
            pointer[a2][min_LRUindex].LRU_time = time;
            pointer[a2][min_LRUindex].Tag = a1;
    
        }
    }
    
}


int main(int argc,char **argv)
{
    char ops;
    char* t = NULL;
            while ((ops = getopt(argc,argv,"hvs:E:b:t:")) != -1)
    {
            switch (ops)
            {
            case  's':
                s = atoi(optarg);
                break;
            
            case 'E':
                E = atoi(optarg);
                break;

            case 'b':
                b = atoi(optarg);
                break;

            case 't':
                t = optarg;
                break;

            case 'h':
            break;

            case 'v':
            break;

            default:
            return -1;

            }
    }

        Set_count = 1u << s;
        if (Set_count == 1)
        {
            return -1;
        }
        cache_init(Set_count,E);
    

        
        FILE *fp = fopen(t,"r");
        char arr[512] = {0};
        char op_mod = 0;
        unsigned long address = 0;
        int size = 0;

        while (fgets(arr,sizeof(arr),fp) != NULL)
        {   
            if(sscanf(arr," %c %lx,%d",&op_mod,&address,&size) != 3)
            {
                    printf("String is no match");
                    continue;
            }
            switch (op_mod)
            {
            case 'I':
                break;

            case 'S':
            case 'L':
            access_cache(address);
                break;

            case 'M':
            access_cache(address);
            access_cache(address);
                break;

            }

        }

            // 1. 先释放每一组（Set）
        for (int i = 0; i < Set_count; i++) 
        {
            free(pointer[i]); 
        }

            // 2. 最后释放指针数组本身
            free(pointer);
        
            printSummary(hit_count, miss_count, change_count);
            return 0;
}

