#include <stb/stb_image.h>

#include <nonstd.h>
#include "nonstd_opengl_texture.h"

freelist_t texture_units;
freelist_t loaded_texture_list;
hashmap_t loaded_textures_map;
GLint MaxTextureImageUnits;

int texture_unit_freelist_alloc()
{
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureImageUnits); // fragment
    freelist_alloc(&texture_units, MaxTextureImageUnits, sizeof(unsigned int));
    return 0;
}

int texture_unit_freelist_free()
{
    freelist_free(&texture_units);
    return 0;
}

int loaded_textures_alloc(unsigned long num_textures)
{
    freelist_alloc(&loaded_texture_list, num_textures, sizeof(texture_t));
    hashmap_alloc(&loaded_textures_map, NULL, num_textures, num_textures);
    return 0;
}

int loaded_textures_free()
{
    freelist_free(&loaded_texture_list);
    hashmap_free(&loaded_textures_map);
    return 0;
}

int texture_alloc(texture_t *texture, const char *filePath)
{

    pthread_mutex_lock(&(texture->mutex_lock));
    texture->width = -1;
    texture->height = -1;
    texture->channels = -1;
    texture->ID = GL_FALSE;
    texture->unit = -1;

    unsigned char *data = stbi_load(filePath, &(texture->width), &(texture->height), &(texture->channels), 0);
    if (!data)
    {
        pthread_mutex_unlock(&(texture->mutex_lock));
        fprintf(stderr, "%s\n", filePath);
        stbi_image_free(data);
        return 1;
    }

    glGenTextures(1, &(texture->ID));
    GLenum format = GL_FALSE;
    if (texture->channels == 1)
        format = GL_RED;
    else if (texture->channels == 3)
        format = GL_RGB;
    else if (texture->channels == 4)
        format = GL_RGBA;

    GLint prevTexture2D;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture2D);

    glBindTexture(GL_TEXTURE_2D, texture->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, prevTexture2D);

    pthread_mutex_unlock(&(texture->mutex_lock));
    return 0;
}

int texture_free(texture_t *texture)
{

    pthread_mutex_lock(&(texture->mutex_lock));
    glDeleteTextures(1, &(texture->ID));
    texture->width = -1;
    texture->height = -1;
    texture->channels = -1;
    texture->ID = GL_FALSE;
    texture->unit = -1;
    pthread_mutex_unlock(&(texture->mutex_lock));
    return 0;
}

int texture_activate(unsigned long int loaded_texture_index, int * unit)
{
    texture_t *texture;

    pthread_rwlock_rdlock(&(loaded_texture_list.pool.rwlock));

    if (loaded_texture_index > loaded_texture_list.pool.max_count)
    {
        pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock));
        return 1;
    }

    pool_get_ptr((void **)&texture, &(loaded_texture_list.pool), loaded_texture_index);
    pthread_mutex_lock(&(texture->mutex_lock));
    pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock));

    if (texture->unit == -1)
    {
        unsigned long int new_unit = (unsigned int)-1l;
        freelist_aquire(&new_unit, &texture_units);
        texture->unit = (int)new_unit;

        GLint prevTextureUnit;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &prevTextureUnit);

        glActiveTexture(GL_TEXTURE0 + texture->unit);
        glBindTexture(GL_TEXTURE_2D, texture->ID);

        glActiveTexture(prevTextureUnit);
    }
    *unit = texture->unit;

    pthread_mutex_unlock(&(texture->mutex_lock));

    return 0;
}

int texture_deactivate(unsigned long int loaded_texture_index)
{
    texture_t *texture;

    pthread_rwlock_rdlock(&(loaded_texture_list.pool.rwlock));
    if (loaded_texture_index > loaded_texture_list.pool.max_count)
    {
        pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock));
        return 1;
    }

    pool_get_ptr((void **)&texture, &(loaded_texture_list.pool), loaded_texture_index);
    pthread_mutex_lock(&(texture->mutex_lock));
    pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock));

    if (texture->unit != -1)
    {
        GLint prevTextureUnit;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &prevTextureUnit);

        glActiveTexture(GL_TEXTURE0 + texture->unit);
        glBindTexture(GL_TEXTURE_2D, GL_FALSE);

        glActiveTexture(prevTextureUnit);

        unsigned long int unit = (unsigned long int)texture->unit;
        freelist_release(&unit, &texture_units);
        texture->unit = (int)-1;
    }

    pthread_mutex_unlock(&(texture->mutex_lock));
    return 0;
}

int get_load_texture(unsigned long int *loaded_texture_index, char *filePath, unsigned long path_length)
{

    texture_t *texture;

    hashmap_find((void **)loaded_texture_index, &loaded_textures_map, (unsigned char *)filePath, path_length);

    pthread_rwlock_rdlock(&(loaded_texture_list.pool.rwlock));
    if (*loaded_texture_index > loaded_texture_list.pool.max_count)
    {
        freelist_aquire(loaded_texture_index, &loaded_texture_list);
        pool_get_ptr((void **)&texture, &(loaded_texture_list.pool), *loaded_texture_index);
        pthread_mutex_lock(&(texture->mutex_lock));
        pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock));

        texture_alloc(texture, filePath);
        pthread_mutex_unlock(&(texture->mutex_lock));

        hashmap_add((void *)loaded_texture_index, &loaded_textures_map, (unsigned char *)filePath, path_length);
    }
    else
    {
        pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock));
    }

    return 0;
}

int unload_texture(char *filePath, unsigned long path_length)
{
    unsigned long int loaded_texture_index = (unsigned long int)-1l;
    texture_t *texture;

    hashmap_remove((void **)&loaded_texture_index, &loaded_textures_map, (unsigned char *)filePath, path_length);

    pthread_rwlock_rdlock(&(loaded_texture_list.pool.rwlock));
    if (loaded_texture_index < loaded_texture_list.pool.max_count)
    {
        pool_get_ptr((void **)&texture, &(loaded_texture_list.pool), loaded_texture_index);
        pthread_mutex_lock(&(texture->mutex_lock));
        pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock));
        texture_free(texture);
        pthread_mutex_unlock(&(texture->mutex_lock));

        freelist_release(&loaded_texture_index, &loaded_texture_list);
    }
    else
    {
        pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock));
    }
    return 0;
}
