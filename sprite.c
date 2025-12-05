//
// Created by Alan Pitcher on 12/1/2025.
//
#include "sprite.h"
int spriteOrder[NUM_SPRITES];
double spriteDistance[NUM_SPRITES];
Sprite sprite[NUM_SPRITES] = {
    //greenlights

    {3.5,1.5,4},
    {10.5,1.5,4},
    {7.5,7.5,4},
    {15.5,7.5,4},
    {6.5,17.5,4},
    {12.5,21.5,4},

    //barrels
    {2.5,5.5,5},
    {4.5,5.5,5},
    {9.5,9.5,5},
    {14.5,9.5,5},
    {3.5,13.5,5},
    {20.5,13.5,5},

    //pillars
    {3.5,16.5,6},
    {5.5,16.5,6},
    {3.5,18.5,6},
    {5.5,18.5,6},
    {3.5,20.5,6},
    {5.5,20.5,6},
    {19,18,6}
};

void sort_sprites(int* order, double* distance, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = i + 1 ; j < n; j++) {
            if (distance[i] < distance[j]) {
                double temp_d = distance[i];
                distance[i] = distance[j];
                distance[j] = temp_d;

                int temp_o = order[i];
                order[i] = order[j];
                order[j] = temp_o;
            }
        }
    }
}
