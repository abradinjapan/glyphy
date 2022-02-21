#ifndef __GLYPHY__
#define __GLYPHY__

/* Include */
// graphics
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>

// allocation
#include <stdlib.h>

/* Define */
typedef unsigned long long u64;

/* Allocate */
void* GLY__allocation__allocate(u64 length) {
    return malloc(length);
}

u64 GLY__allocation__deallocate(void* data, u64 length) {
    free(data);

    return 0;
}

/* Error */
typedef enum GLY__et {
    // no error
    GLY__et__no_error,

    // buffer
    GLY__et__allocation_denied
} GLY__et;

typedef struct GLY__error {
    GLY__et p_type;
    u64 p_extra_datum;
} GLY__error;

GLY__error GLY__create__error(GLY__et type) {
    GLY__error output;

    output.p_type = type;
    output.p_extra_datum = 0;

    return output;
}

/* Buffer */
typedef struct GLY__buffer {
    void* p_data;
    u64 p_length;
} GLY__buffer;

GLY__buffer GLY__create_null__buffer() {
    GLY__buffer output;

    output.p_data = 0;
    output.p_length = 0;

    return output;
}

GLY__buffer GLY__create_custom__buffer(void* data, u64 length) {
    GLY__buffer output;

    output.p_data = data;
    output.p_length = length;

    return output;
}

GLY__buffer GLY__create__buffer(GLY__error* error, u64 length) {
    GLY__buffer output;

    output.p_data = GLY__allocation__allocate(length);
    if (output.p_data == 0) {
        *error = GLY__create__error(GLY__et__allocation_denied);

        output.p_length = 0;
    } else {
        output.p_length = length;
    }

    return output;
}

void GLY__destroy__buffer(GLY__buffer buffer) {
    GLY__allocation__deallocate(buffer.p_data, buffer.p_length);

    return;
}

#endif
