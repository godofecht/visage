#include "embedded/shaders.h"

int main() {
  if (visage::shaders::vs_probe.size <= 0) {
    return 1;
  }
  if (visage::shaders::fs_probe.size <= 0) {
    return 1;
  }
  if (visage::shaders::vs_probe.data[0] != 'v') {
    return 1;
  }
  if (visage::shaders::fs_probe.data[0] != 'f') {
    return 1;
  }
  return 0;
}
