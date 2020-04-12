#include <GL/glew.h>
#include <stdio.h>

int main() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        printf("GLEW error\n");
    printf("Vendor: %s\n", glGetString(GL_VENDOR));
}