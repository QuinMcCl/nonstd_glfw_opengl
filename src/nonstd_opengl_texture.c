#include <GL/glew.h>

#include <stb/stb_image.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <nonstd.h>
#include "nonstd_opengl_texture.h"

pthread_mutex_t lock_texture_unit_array = {0};
long int max_texture_image_units = 0;
long int index_texture_image_unit = -1;
long int *texture_image_unit_array = NULL;

texture_t *loaded_texture_array = NULL;
unsigned long max_num_textures = 0;
long int index_texture = -1;

freelist_t loaded_texture_list = {0};

hashmap_t loaded_textures_map = {0};
hash_node_t **hash_node_ptr_array = NULL;
hash_node_t *hash_node_array = NULL;

int textureUnits_initialized = GL_FALSE;
int LoadedTextures_initialized = GL_FALSE;

#define ON_ERROR return errno;
int texture_unit_freelist_alloc()
{
    if (textureUnits_initialized == GL_TRUE)
        return 0;

    glGetInteger64v(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_image_units); // fragment

    CHECK_ERR(max_texture_image_units <= 0
                  ? EPERM
                  : EXIT_SUCCESS);
    CHECK_ERR(safe_alloc((void **)&texture_image_unit_array, max_texture_image_units * sizeof(texture_image_unit_array[0])));
    memset(texture_image_unit_array, 0, max_texture_image_units * sizeof(texture_image_unit_array[0]));

    for (long int index = 0; index < max_texture_image_units; index++)
    {
        texture_image_unit_array[index] = index;
    }
    index_texture_image_unit = 0;

    textureUnits_initialized = GL_TRUE;

    return 0;
}

int texture_unit_freelist_free()
{
    if (textureUnits_initialized != GL_TRUE)
        return 0;

    CHECK_ERR(safe_free((void **)&texture_image_unit_array));

    index_texture_image_unit = -1;
    max_texture_image_units = 0;
    textureUnits_initialized = GL_FALSE;
    return 0;
}

int loaded_textures_alloc(unsigned long num_textures)
{
    CHECK_ERR(num_textures == 0 ? EINVAL : EXIT_SUCCESS);
    if (LoadedTextures_initialized == GL_TRUE)
        return 0;
    stbi_set_flip_vertically_on_load(1);

    // CHECK(freelist_alloc(&loaded_texture_list, num_textures, sizeof(texture_t)), return errno);

    CHECK_ERR(safe_alloc((void **)&loaded_texture_array, num_textures * sizeof(loaded_texture_array[0])));
    memset(loaded_texture_array, 0, num_textures * sizeof(loaded_texture_array[0]));

    CHECK_ERR(freelist_init(&loaded_texture_list,
                            num_textures * sizeof(loaded_texture_array[0]),
                            loaded_texture_array,
                            sizeof(loaded_texture_array[0]),
                            8UL,
                            NULL,
                            NULL));

    CHECK_ERR(safe_alloc((void **)&hash_node_ptr_array, num_textures * sizeof(hash_node_ptr_array[0])));
    memset(hash_node_ptr_array, 0, num_textures * sizeof(hash_node_ptr_array[0]));
    CHECK_ERR(safe_alloc((void **)&hash_node_array, num_textures * sizeof(hash_node_array[0])));
    memset(hash_node_array, 0, num_textures * sizeof(hash_node_array[0]));
    CHECK_ERR(hashmap_init(
        &loaded_textures_map,
        num_textures,
        hash_node_ptr_array,
        num_textures,
        hash_node_array,
        NULL,
        NULL,
        NULL,
        NULL));
    max_num_textures = num_textures;
    LoadedTextures_initialized = GL_TRUE;
    return 0;
}

int loaded_textures_free()
{
    if (LoadedTextures_initialized != GL_TRUE)
        return 0;
    CHECK_ERR(safe_free((void **)&loaded_texture_array));

    CHECK_ERR(safe_free((void **)&hash_node_ptr_array));
    CHECK_ERR(safe_free((void **)&hash_node_array));

    LoadedTextures_initialized = GL_FALSE;
    return 0;
}

int texture_alloc(texture_t *texture, const char *filePath)
{
    CHECK_ERR(texture == NULL || filePath == NULL ? EINVAL : EXIT_SUCCESS);

    CHECK_ERR(pthread_mutex_lock(&(texture->mutex_lock)));

    texture->width = -1;
    texture->height = -1;
    texture->channels = -1;
    texture->ID = GL_FALSE;
    texture->unit = -1;

    unsigned char *data = stbi_load(filePath, &(texture->width), &(texture->height), &(texture->channels), 0);
    if (data == NULL)
    {
        CHECK_ERR(pthread_mutex_unlock(&(texture->mutex_lock)));
        return -1;
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

    CHECK_ERR(pthread_mutex_unlock(&(texture->mutex_lock)));
    return 0;
}

int texture_free(texture_t *texture)
{
    CHECK_ERR(texture == NULL ? EINVAL : EXIT_SUCCESS);

    CHECK_ERR(pthread_mutex_lock(&(texture->mutex_lock)));
    glDeleteTextures(1, &(texture->ID));
    texture->width = -1;
    texture->height = -1;
    texture->channels = -1;
    texture->ID = GL_FALSE;
    texture->unit = -1;
    CHECK_ERR(pthread_mutex_unlock(&(texture->mutex_lock)));
    return 0;
}

int texture_activate(texture_t *texture, int *unit)
{
    CHECK_ERR(texture == NULL || unit == NULL ? EINVAL : EXIT_SUCCESS);

    CHECK_ERR(pthread_mutex_lock(&(texture->mutex_lock)));

    if (texture->unit == -1)
    {
        long int new_unit = -1l;

        CHECK_ERR(pthread_mutex_lock(&(lock_texture_unit_array)));
        if (index_texture_image_unit < 0 || index_texture_image_unit >= max_texture_image_units)
        {
            CHECK_ERR(pthread_mutex_unlock(&(lock_texture_unit_array)));
            return -1;
        }
        new_unit = texture_image_unit_array[index_texture_image_unit++];

        CHECK_ERR(pthread_mutex_unlock(&(lock_texture_unit_array)));

        texture->unit = (int)new_unit;

        GLint prevTextureUnit;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &prevTextureUnit);

        glActiveTexture(GL_TEXTURE0 + texture->unit);
        glBindTexture(GL_TEXTURE_2D, texture->ID);

        glActiveTexture(prevTextureUnit);
    }
    *unit = texture->unit;

    CHECK_ERR(pthread_mutex_unlock(&(texture->mutex_lock)));

    return 0;
}

int texture_deactivate(texture_t *texture)
{
    CHECK_ERR(texture == NULL ? EINVAL : EXIT_SUCCESS);

    CHECK_ERR(pthread_mutex_lock(&(texture->mutex_lock)));

    if (texture->unit != -1)
    {
        GLint prevTextureUnit;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &prevTextureUnit);

        glActiveTexture(GL_TEXTURE0 + texture->unit);
        glBindTexture(GL_TEXTURE_2D, GL_FALSE);

        glActiveTexture(prevTextureUnit);

        CHECK_ERR(pthread_mutex_lock(&(lock_texture_unit_array)));
        if (index_texture_image_unit <= 0 || index_texture_image_unit > max_texture_image_units)
        {
            CHECK_ERR(pthread_mutex_unlock(&(lock_texture_unit_array)));
            return -1;
        }
        texture_image_unit_array[--index_texture_image_unit] = texture->unit;

        CHECK_ERR(pthread_mutex_unlock(&(lock_texture_unit_array)));

        texture->unit = (int)-1;
    }

    CHECK_ERR(pthread_mutex_unlock(&(texture->mutex_lock)));
    return 0;
}

int get_load_texture(texture_t **ptr_texture, char *filePath, unsigned long path_length)
{
    CHECK_ERR(ptr_texture == NULL || filePath == NULL ? EINVAL : EXIT_SUCCESS);

    texture_t *texture = NULL;

    CHECK_ERR(HASHMAP_FIND(loaded_textures_map, path_length, filePath, texture));

    if (texture == NULL)
    {
        FREELIST_GET(loaded_texture_list, texture);

        memset(texture, 0, sizeof(*texture));

        CHECK_ERR(texture_alloc(texture, filePath));

        CHECK_ERR(HASHMAP_ADD(loaded_textures_map, path_length, filePath, texture));
    }
    *ptr_texture = texture;
    return 0;
}

int unload_texture(char *filePath, unsigned long path_length)
{
    texture_t *texture = NULL;
    CHECK_ERR(HASHMAP_ADD(loaded_textures_map, path_length, filePath, texture));

    if (texture != NULL)
    {
        CHECK_ERR(texture_free(texture));
        FREELIST_REL(loaded_texture_list, texture);
    }
    return 0;
}

#undef ON_ERROR