#include <GL/glew.h>
#include <GL/glut.h>

#include <stb/stb_image.h>
#include <string.h>

#include <nonstd.h>
#include "nonstd_opengl_texture.h"

freelist_t texture_units;
freelist_t loaded_texture_list;
hashmap_t loaded_textures_map;

int textureUnits_initialized = GL_FALSE;
int LoadedTextures_initialized = GL_FALSE;

int texture_unit_freelist_alloc()
{
    if (textureUnits_initialized == GL_TRUE)
        return 0;

    long int MaxTextureImageUnits;
    glGetInteger64v(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureImageUnits); // fragment

#ifdef ERROR_CHECKING
    THROW_ERR((MaxTextureImageUnits <= 0), "INVALID MAX TextureUnits POINTS", return retval);
#endif
    CHECK(freelist_alloc(&texture_units, MaxTextureImageUnits, sizeof(unsigned long int)), return retval);
    textureUnits_initialized = GL_TRUE;

    return 0;
}

int texture_unit_freelist_free()
{
    if (textureUnits_initialized != GL_TRUE)
        return 0;

    CHECK(freelist_free(&texture_units), return retval);

    textureUnits_initialized = GL_FALSE;
    return 0;
}

int loaded_textures_alloc(unsigned long num_textures)
{
#ifdef ERROR_CHECKING
    THROW_ERR((num_textures == 0), "INVALID TEXTURE COUNT", return retval);
#endif

    if (LoadedTextures_initialized == GL_TRUE)
        return 0;
    CHECK(freelist_alloc(&loaded_texture_list, num_textures, sizeof(texture_t)), return retval);
    CHECK(hashmap_alloc(&loaded_textures_map, NULL, num_textures, num_textures), return retval);
    LoadedTextures_initialized = GL_TRUE;
    return 0;
}

int loaded_textures_free()
{
    if (LoadedTextures_initialized != GL_TRUE)
        return 0;
    CHECK(freelist_free(&loaded_texture_list), return retval);
    CHECK(hashmap_free(&loaded_textures_map), return retval);
    LoadedTextures_initialized = GL_FALSE;
    return 0;
}

int texture_alloc(texture_t *texture, const char *filePath)
{
#ifdef ERROR_CHECKING
    THROW_ERR((texture == NULL), "NULL TEXTURE PTR", return retval);
    THROW_ERR((filePath == NULL), "NULL FILE PTR", return retval);
#endif

    THROW_ERR(pthread_mutex_lock(&(texture->mutex_lock)), strerror(errno), return retval);

    texture->width = -1;
    texture->height = -1;
    texture->channels = -1;
    texture->ID = GL_FALSE;
    texture->unit = -1;

    unsigned char *data = stbi_load(filePath, &(texture->width), &(texture->height), &(texture->channels), 0);
    THROW_ERR((data == NULL), "",
              {
                  THROW_ERR(pthread_mutex_unlock(&(texture->mutex_lock)), strerror(errno), return retval);
                  return retval;
              });

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

    THROW_ERR(pthread_mutex_unlock(&(texture->mutex_lock)), strerror(errno), return retval);
    return 0;
}

int texture_free(texture_t *texture)
{
#ifdef ERROR_CHECKING
    THROW_ERR((texture == NULL), "NULL TEXTURE PTR", return retval);
#endif

    THROW_ERR(pthread_mutex_lock(&(texture->mutex_lock)), strerror(errno), return retval);
    glDeleteTextures(1, &(texture->ID));
    texture->width = -1;
    texture->height = -1;
    texture->channels = -1;
    texture->ID = GL_FALSE;
    texture->unit = -1;
    THROW_ERR(pthread_mutex_unlock(&(texture->mutex_lock)), strerror(errno), return retval);
    return 0;
}

int texture_activate(unsigned long int loaded_texture_index, int *unit)
{
    THROW_ERR(pthread_rwlock_rdlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
#ifdef ERROR_CHECKING
    THROW_ERR((loaded_texture_index > loaded_texture_list.pool.max_count), "INVALID TEXTURE INDEX", {
        THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
        return retval;
    });
#endif
    texture_t *texture;

    CHECK(pool_get_ptr((void **)&texture, &(loaded_texture_list.pool), loaded_texture_index), {
        THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
        return retval;
    });
    THROW_ERR(pthread_mutex_lock(&(texture->mutex_lock)), strerror(errno), {
        THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
        return retval;
    });
    THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), {
        THROW_ERR(pthread_mutex_unlock(&(texture->mutex_lock)), strerror(errno), return retval);
        return retval;
    });

    if (texture->unit == -1)
    {
        unsigned long int new_unit = (unsigned int)-1l;
        CHECK(freelist_aquire(&new_unit, &texture_units), {
            THROW_ERR(pthread_mutex_unlock(&(texture->mutex_lock)), strerror(errno), return retval);
            return retval;
        });
        texture->unit = (int)new_unit;

        GLint prevTextureUnit;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &prevTextureUnit);

        glActiveTexture(GL_TEXTURE0 + texture->unit);
        glBindTexture(GL_TEXTURE_2D, texture->ID);

        glActiveTexture(prevTextureUnit);
    }
    *unit = texture->unit;

    THROW_ERR(pthread_mutex_unlock(&(texture->mutex_lock)), strerror(errno), return retval);

    return 0;
}

int texture_deactivate(unsigned long int loaded_texture_index)
{

    THROW_ERR(pthread_rwlock_rdlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
#ifdef ERROR_CHECKING
    THROW_ERR((loaded_texture_index > loaded_texture_list.pool.max_count), "INVALID TEXTURE INDEX", {
        THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
        return retval;
    });
#endif

    texture_t *texture;

    CHECK(pool_get_ptr((void **)&texture, &(loaded_texture_list.pool), loaded_texture_index), {
        THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
        return retval;
    });
    THROW_ERR(pthread_mutex_lock(&(texture->mutex_lock)), strerror(errno), {
        THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
        return retval;
    });
    THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), {
        THROW_ERR(pthread_mutex_unlock(&(texture->mutex_lock)), strerror(errno), return retval);
        return retval;
    });

    if (texture->unit != -1)
    {
        GLint prevTextureUnit;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &prevTextureUnit);

        glActiveTexture(GL_TEXTURE0 + texture->unit);
        glBindTexture(GL_TEXTURE_2D, GL_FALSE);

        glActiveTexture(prevTextureUnit);

        unsigned long int unit = (unsigned long int)texture->unit;
        CHECK(freelist_release(&unit, &texture_units), {
            THROW_ERR(pthread_mutex_unlock(&(texture->mutex_lock)), strerror(errno), return retval);
            return retval;
        });
        texture->unit = (int)-1;
    }

    THROW_ERR(pthread_mutex_unlock(&(texture->mutex_lock)), strerror(errno), return retval);
    return 0;
}

int get_load_texture(unsigned long int *loaded_texture_index, char *filePath, unsigned long path_length)
{

    texture_t *texture;

    CHECK(hashmap_find((void **)loaded_texture_index, &loaded_textures_map, (unsigned char *)filePath, path_length), return retval);

    THROW_ERR(pthread_rwlock_rdlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);

    if (*loaded_texture_index > loaded_texture_list.pool.max_count)
    {
        CHECK(freelist_aquire(loaded_texture_index, &loaded_texture_list), {
            THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
            return retval;
        });
        CHECK(pool_get_ptr((void **)&texture, &(loaded_texture_list.pool), *loaded_texture_index), {
            THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
            return retval;
        });

        CHECK(texture_alloc(texture, filePath), {
            THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
            return retval;
        });

        CHECK(hashmap_add((void *)loaded_texture_index, &loaded_textures_map, (unsigned char *)filePath, path_length), {
            THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
            return retval;
        });
    }
    THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);

    return 0;
}

int unload_texture(char *filePath, unsigned long path_length)
{
    unsigned long int loaded_texture_index = (unsigned long int)-1l;
    texture_t *texture;

    CHECK(hashmap_remove((void **)&loaded_texture_index, &loaded_textures_map, (unsigned char *)filePath, path_length), return retval);

    THROW_ERR(pthread_rwlock_rdlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
    if (loaded_texture_index < loaded_texture_list.pool.max_count)
    {
        CHECK(pool_get_ptr((void **)&texture, &(loaded_texture_list.pool), loaded_texture_index), {
            THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
            return retval;
        });
        CHECK(texture_free(texture), {
            THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
            return retval;
        });
        CHECK(freelist_release(&loaded_texture_index, &loaded_texture_list), {
            THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
            return retval;
        });
    }
    THROW_ERR(pthread_rwlock_unlock(&(loaded_texture_list.pool.rwlock)), strerror(errno), return retval);
    return 0;
}
