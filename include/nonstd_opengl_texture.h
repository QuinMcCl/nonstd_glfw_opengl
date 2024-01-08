#ifndef NONSTD_OPENGL_TEXTURE_H
#define NONSTD_OPENGL_TEXTURE_H


// #include <string>
// #include <map>


typedef struct texture_s
{
    unsigned int ID;
    int unit;
    int width;
    int height;
    int channels;
    pthread_mutex_t mutex_lock;
} texture_t;


int texture_unit_freelist_alloc();
int texture_unit_freelist_free();

int loaded_textures_alloc(unsigned long num_textures);
int loaded_textures_free();


int texture_alloc(texture_t * texture, const char * filePath);
int texture_free(texture_t *texture);
int texture_activate(unsigned long int loaded_texture_index, int* unit);
int texture_deactivate(unsigned long int loaded_texture_index);


int get_load_texture(unsigned long int *loaded_texture_index, char *filePath, unsigned long path_length);
int unload_texture(char *filePath, unsigned long path_length);


#endif // NONSTD_OPENGL_TEXTURE_H