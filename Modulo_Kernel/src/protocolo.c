#include "protocolo.h"



static void deserializar_instructions(void* stream, char* instructions) {
    memcpy(instructions, stream, sizeof(char*));
}


bool recv_instructions(int fd, char* instructions) {
    size_t size = sizeof(char*);
    void* stream = malloc(size);

    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    deserializar_instructions(stream, instructions);

    free(stream);
    return true;
}
