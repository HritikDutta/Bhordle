#include "fileio.h"

#include "core/logging.h"
#include "containers/string.h"
#include "containers/stringview.h"
#include "platform/platform.h"

void LoadFile(const StringView& filepath, String& output)
{
    FILE* file = fopen(filepath.cstr(), "rb");
    Assert(file != nullptr);

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*) PlatformAllocate((length + 1) * sizeof(char));
    fread(buffer, sizeof(char), length, file);
    buffer[length] = '\0';

    output = buffer;
    free(buffer);

    fclose(file);
}