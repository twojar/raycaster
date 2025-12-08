//
// Created by Alan Pitcher on 12/1/2025.
//
#include "sprite.h"
int spriteOrder[NUM_SPRITES];
double spriteDistance[NUM_SPRITES];
Sprite sprite[NUM_SPRITES] = {
    //greenlights
    {.x = 1.5,  .y = 3.5,  .texture = 4},
    {.x = 1.5,  .y = 10.5, .texture = 4},
    {.x = 7.5,  .y = 7.5,  .texture = 4},
    {.x = 7.5,  .y = 15.5, .texture = 4},
    {.x = 17.5, .y = 6.5,  .texture = 4},
    {.x = 21.5, .y = 12.5, .texture = 4},


    //barrels
    { .x = 5.5,  .y = 2.5,  .texture = 5 },
    { .x = 5.5,  .y = 4.5,  .texture = 5 },
    { .x = 9.5,  .y = 9.5,  .texture = 5 },
    { .x = 9.5,  .y = 14.5, .texture = 5 },
    { .x = 13.5, .y = 3.5,  .texture = 5 },
    { .x = 13.5, .y = 20.5, .texture = 5 },


    //pillars
    { .x = 16.5, .y = 3.5,  .texture = 6 },
    { .x = 16.5, .y = 5.5,  .texture = 6 },
    { .x = 18.5, .y = 3.5,  .texture = 6 },
    { .x = 18.5, .y = 5.5,  .texture = 6 },
    { .x = 20.5, .y = 3.5,  .texture = 6 },
    { .x = 20.5, .y = 5.5,  .texture = 6 },
    { .x = 18.0, .y = 19.0, .texture = 6 },

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
