#include <stdio.h>
#include <stdlib.h>
#include <math.h>
void main()
{
    int i;
    int dry_run[] = {1, 4, 1, 2, 1, 4, 1, 1, 3, 2, 1, 3, 2, 1, 2, 4, 3, 2, 3, 3, 4, 3, 2, 1, 2, 1, 4, 1, 1, 3, 2};
    int final_run[100] = {};
    int size = sizeof(dry_run) / sizeof(dry_run[0]);
    int prev_index = 0;
    int prev_value = dry_run[prev_index];
   
    int x = 0;

    for (int i = 1; i < size; i++)
    {


        if (abs(prev_value - dry_run[i]) == 2)
        {
            if(dry_run[prev_index] !=0){
        
                while((abs(dry_run[prev_index -1] - dry_run[i+1])==2)&&(dry_run[prev_index - 1] != 0)){

                    dry_run[prev_index -1] = 0;
                    dry_run[i+1] =0;
                    
                }
            
            
                dry_run[prev_index] = 0;
                dry_run[i] = 0;
                prev_index++;
                prev_value = dry_run[prev_index];
            }
            else{
                prev_index++;
             prev_value = dry_run[prev_index];
            }
          
        }
        
        else{
            prev_index++;
            prev_value = dry_run[prev_index];
        }
    }

    for(int i =0;i<size;i++){
              if(dry_run[i]!=0){
                final_run[x]=dry_run[i];
                x ++;
              }
              else{
                continue;
              }

    }

    for(int i =0 ;i<x;i++){
        printf("%d ",final_run[i]);
}
}
