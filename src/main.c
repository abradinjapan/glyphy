#include <stdio.h>

#include "glyphy.h"

int main() {
    GLY__error error;

    // setup error
    error = GLY__create__error(GLY__et__no_error);

    // print title
    printf("Glyphy!\n");

    // run game
    GLY__run(&error, 1080, 720);

    // print error
    GLY__print__error(&error);

    // clear error
    GLY__destroy__error(&error);

    return 0;
}
