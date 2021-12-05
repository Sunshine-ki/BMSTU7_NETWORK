#include "file_server.h"

#define PORT 3001

int main() {
    FileServer server(PORT);
    server.start_processing();
    return 0;
}
