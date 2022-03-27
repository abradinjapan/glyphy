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
// define type
typedef enum GLY__dt {
    GLY__dt__opengl_error_info_log_length = 1024
} GLY__dt;

// custom type names
typedef unsigned long long u64;
typedef char s8;

/* Boolean */
// boolean type
typedef enum GLY__bt {
    GLY__bt__false,
    GLY__bt__true,
} GLY__bt;

/* Copy Bytes */
void GLY__copy__bytes_to_bytes(void* source, u64 length, void* destination) {
    for (u64 i = 0; i < length; i++) {
        ((unsigned char*)destination)[i] = ((unsigned char*)source)[i];
    }

    return;
}

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
    GLY__et__allocation_denied,

    // other opengl error
    GLY__et__other_opengl,

    // graphics
    GLY__et__sdl2_initialization_failure,
    GLY__et__opengl_window_initialization_failure,
    GLY__et__opengl_context_initialization_failure,
    GLY__et__glew_initialization_failure,

    // shaders
    GLY__et__vertex_shader_compilation_failure,
    GLY__et__fragment_shader_compilation_failure,
    GLY__et__shader_linking_failure
} GLY__et;

typedef struct GLY__error {
    GLY__et p_type;
    u64 p_extra_datum;
    s8* p_opengl_log;
} GLY__error;

GLY__error GLY__create__error(GLY__et type) {
    GLY__error output;

    output.p_type = type;
    output.p_extra_datum = 0;
    output.p_opengl_log = 0;

    return output;
}

GLY__error GLY__create__error__vertex_shader_compilation_failure(s8* opengl_log, GLenum opengl_error_code) {
    GLY__error output;

    // setup output
    output.p_type = GLY__et__vertex_shader_compilation_failure;
    output.p_extra_datum = opengl_error_code;
    output.p_opengl_log = opengl_log;
    
    return output;
}

GLY__error GLY__create__error__fragment_shader_compilation_failure(s8* opengl_log, GLenum opengl_error_code) {
    GLY__error output;

    // setup output
    output.p_type = GLY__et__fragment_shader_compilation_failure;
    output.p_extra_datum = opengl_error_code;
    output.p_opengl_log = opengl_log;
    
    return output;
}

GLY__error GLY__create__error__shader_linking_failure(s8* opengl_log, GLenum opengl_error_code) {
    GLY__error output;

    // setup output
    output.p_type = GLY__et__shader_linking_failure;
    output.p_extra_datum = opengl_error_code;
    output.p_opengl_log = opengl_log;
    
    return output;
}

GLY__error GLY__try_create__error__other_opengl() {
    GLY__error output;
    GLenum opengl_error;

    // check error
    opengl_error = glGetError();

    if (opengl_error != GL_NO_ERROR) {
        output.p_type = GLY__et__other_opengl;
        output.p_extra_datum = (u64)opengl_error;
        output.p_opengl_log = 0;
    } else {
        return GLY__create__error(GLY__et__no_error);
    }

    return output;
}

GLY__bt GLY__check__error(GLY__error* error) {
    return (GLY__bt)(error == 0);
}

void GLY__destroy__error(GLY__error* error) {
    if ((*error).p_opengl_log != 0) {
        GLY__allocation__deallocate((*error).p_opengl_log, sizeof(s8) * GLY__dt__opengl_error_info_log_length);
    }

    return;
}

void GLY__print__error(GLY__error* error) {
    printf("Glyphy Error Code: %llu\n", (u64)(*error).p_type);

    if ((*error).p_extra_datum != 0) {
        printf("OpenGL Error Code: %llu\n", (u64)(*error).p_extra_datum);
    }

    if ((*error).p_opengl_log != 0) {
        printf("OpenGL Error Log:\n\t%s\n", (*error).p_opengl_log);
    }

    return;
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

GLY__buffer GLY__create__buffer_copy_from_c_string(GLY__error* error, s8* string) {
    GLY__buffer output;

    // setup output
    output = GLY__create__buffer(error, strlen(string) + 1);
    if (GLY__check__error(error) == GLY__bt__true) {
        return output;
    }

    // copy string
    GLY__copy__bytes_to_bytes(string, output.p_length, output.p_data);

    return output;
}

void GLY__destroy__buffer(GLY__buffer buffer) {
    GLY__allocation__deallocate(buffer.p_data, buffer.p_length);

    return;
}

/* Graphics - Getting Window Open */
typedef struct GLY__window_configuration {
    GLY__buffer p_title;
    int p_width;
    int p_height;
    unsigned short p_flags; // currently unused
} GLY__window_configuration;

typedef struct GLY__graphics {
    SDL_Window* p_window_context;
    SDL_GLContext p_sdl2_opengl_context;
} GLY__graphics;

GLY__graphics GLY__create_null__graphics() {
    GLY__graphics output;

    // setup output
    output.p_window_context = 0;
    output.p_sdl2_opengl_context = 0;

    return output;
}

GLY__window_configuration GLY__create__window_configuration(GLY__buffer title_string_pointer, int width, int height, unsigned short flags) {
    GLY__window_configuration output;

    // setup output
    output.p_title = title_string_pointer;
    output.p_width = width;
    output.p_height = height;
    output.p_flags = flags;

    return output;
}

void GLY__destroy__window_configuration(GLY__window_configuration window_configuration) {
    return;
}

GLY__graphics GLY__open__graphics__new_window(GLY__error* error, GLY__window_configuration window_configuration) {
    GLY__graphics output;

    // setup output
    output = GLY__create_null__graphics();

    // initialize SDL2
    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0) {
        *error = GLY__create__error(GLY__et__sdl2_initialization_failure);

        return output;
    }

    // initalize window
    output.p_window_context = SDL_CreateWindow((const s8*)window_configuration.p_title.p_data, 0, 0, window_configuration.p_width, window_configuration.p_height, SDL_WINDOW_OPENGL);
    if (output.p_window_context == 0) {
        *error = GLY__create__error(GLY__et__opengl_window_initialization_failure);

        return output;
    }

    // initalize opengl context
    output.p_sdl2_opengl_context = SDL_GL_CreateContext(output.p_window_context);
    if (output.p_sdl2_opengl_context == 0) {
        *error = GLY__create__error(GLY__et__opengl_context_initialization_failure);

        return output;
    }

    // initalize glew
    if (glewInit() != GLEW_OK) {
        *error = GLY__create__error(GLY__et__glew_initialization_failure);

        return output;
    }

    return output;
}

void GLY__close__graphics__old_window(GLY__graphics window) {
    SDL_GL_DeleteContext(window.p_sdl2_opengl_context);
    SDL_DestroyWindow(window.p_window_context);
    SDL_Quit();

    return;
}

/* Shaders - Programming The GPU */
typedef struct GLY__shader {
    GLuint p_shader_ID;
    GLY__buffer p_program;
} GLY__shader;

typedef struct GLY__shaders_program {
    GLuint p_program_ID;
    GLY__shader p_vertex_shader;
    GLY__shader p_fragment_shader;
} GLY__shaders_program;

GLY__shader GLY__create_null__shader() {
    GLY__shader output;

    // setup output
    output.p_shader_ID = 0;
    output.p_program = GLY__create_null__buffer();

    return output;
}

GLY__shader GLY__compile__shader(GLY__error* error, GLY__buffer shader_data, GLenum shader_type) {
    GLY__shader output;
    GLint error_log_length;
    s8* opengl_error_log;

    // setup output
    output = GLY__create_null__shader();

    // create shader space
    output.p_shader_ID = glCreateShader(shader_type);

    // send program to gpu
    glShaderSource(output.p_shader_ID, 1, (const GLchar* const*)&shader_data.p_data, NULL);

    // compile shader
    glCompileShader(output.p_shader_ID);

    // check for errors
    glGetShaderiv(output.p_shader_ID, GL_INFO_LOG_LENGTH, &error_log_length);
    if (error_log_length > 0) {
        opengl_error_log = GLY__allocation__allocate(sizeof(s8) * GLY__dt__opengl_error_info_log_length);

        glGetShaderInfoLog(output.p_shader_ID, GLY__dt__opengl_error_info_log_length, NULL, opengl_error_log);

        if (shader_type == GL_VERTEX_SHADER) {
            *error = GLY__create__error__vertex_shader_compilation_failure(opengl_error_log, glGetError());
        } else if (shader_type == GL_FRAGMENT_SHADER) {
            *error = GLY__create__error__fragment_shader_compilation_failure(opengl_error_log, glGetError());
        }
    }

    return output;
}

GLY__shaders_program GLY__compile__shaders_program(GLY__error* error, GLY__buffer vertex_shader, GLY__buffer fragment_shader) {
    GLY__shaders_program output;
    GLint error_log_length;
    s8* opengl_error_log;

    // setup output
    output.p_program_ID = 0;
    output.p_vertex_shader = GLY__create_null__shader();
    output.p_fragment_shader = GLY__create_null__shader();
    
    // compile shaders
    output.p_vertex_shader = GLY__compile__shader(error, vertex_shader, GL_VERTEX_SHADER);
    if (GLY__check__error(error) == GLY__bt__true) {
        return output;
    }

    output.p_fragment_shader = GLY__compile__shader(error, fragment_shader, GL_FRAGMENT_SHADER);
    if (GLY__check__error(error) == GLY__bt__true) {
        return output;
    }

    // create shader program
    output.p_program_ID = glCreateProgram();

    // setup shader linking
    glAttachShader(output.p_program_ID, output.p_vertex_shader.p_shader_ID);
    glAttachShader(output.p_program_ID, output.p_fragment_shader.p_shader_ID);
    
    // link shaders
    glLinkProgram(output.p_program_ID);
    
    // check for errors
    glGetProgramiv(output.p_program_ID, GL_LINK_STATUS, &error_log_length);

    if (error_log_length > 1) {
        opengl_error_log = GLY__allocation__allocate(sizeof(s8) * GLY__dt__opengl_error_info_log_length);

        glGetProgramInfoLog(output.p_program_ID, GLY__dt__opengl_error_info_log_length, NULL, opengl_error_log);

        *error = GLY__create__error__shader_linking_failure(opengl_error_log, glGetError());
    }

    return output;
}

void GLY__use__use_shaders(GLY__shaders_program shaders_program) {
    glUseProgram(shaders_program.p_program_ID);

    return;
}

void GLY__close__shaders(GLY__shaders_program shaders_program) {
    glDeleteShader(shaders_program.p_vertex_shader.p_shader_ID);
    glDeleteShader(shaders_program.p_fragment_shader.p_shader_ID);
    glDeleteProgram(shaders_program.p_program_ID);

    return;
}

/* Events - User Input */
typedef struct GLY__user_input {
    unsigned char p_quit;
} GLY__user_input;

GLY__user_input GLY__create_null__user_input() {
    GLY__user_input output;

    // setup output
    output.p_quit = 0;
    
    return output;
}

GLY__user_input GLY__create__user_input__from_sdl2_events() {
    GLY__user_input output;
    SDL_Event e;
    SDL_Event* e_pointer;

    // create null output
    output = GLY__create_null__user_input();

    // setup variables
    e_pointer = &e;

    // get all events
    while (SDL_PollEvent(e_pointer)) {
        if (e.type == SDL_QUIT) {
            output.p_quit = GLY__bt__true;
        }
    }

    return output;
}

/* Display - Running The Program */
void GLY__run(GLY__error* error, int window_width, int window_height) {
    GLY__graphics graphics;
    GLY__window_configuration window_configuration;
    GLY__buffer title;
    GLY__user_input user_input;

    // create title
    title = GLY__create__buffer_copy_from_c_string(error, "Glyphy");
    if (GLY__check__error(error) == GLY__bt__true) {
        return;
    }

    // create window configuration
    window_configuration = GLY__create__window_configuration(title, window_width, window_height, 0);

    // open window
    graphics = GLY__open__graphics__new_window(error, window_configuration);

    // create shaders
    // TODO

    // setup opengl drawing constants
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // maintain window
    while (GLY__bt__true) {
        // get user input
        user_input = GLY__create__user_input__from_sdl2_events();

        // check if user requests a closed window
        if (user_input.p_quit == GLY__bt__true) {
            break;
        }

        // clear window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        *error = GLY__try_create__error__other_opengl();
        if (GLY__check__error(error) == GLY__bt__true) {
            break;
        }

        // display new buffer
        SDL_GL_SwapWindow(graphics.p_window_context);
    }

    GLY__destroy__buffer(title);
    GLY__destroy__window_configuration(window_configuration);
    GLY__close__graphics__old_window(graphics);

    return;
}

#endif
