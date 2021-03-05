#ifndef CNN_HELPER_FUNCS_H
#define CNN_HELPER_FUNCS_H

#include <stdint.h>

typedef struct 
{
    int face_status;
    int x;
    int y;
    int w;
    int h;
}cnn_output_t;

void load_input(void);
void softmax_layer(void);
uint32_t* get_cnn_buffer();
cnn_output_t* run_cnn(int display_txt, int display_bb);
void startup_cnn();


#endif