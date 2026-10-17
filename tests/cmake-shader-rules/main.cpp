#include "embedded/shaders.h"

int main()
{
    return (visage::shaders::vs_probe.size > 0 &&
            visage::shaders::fs_probe.size > 0 &&
            visage::shaders::vs_probe.data[0] == 'v' &&
            visage::shaders::fs_probe.data[0] == 'f') ? 0 : 1;
}
