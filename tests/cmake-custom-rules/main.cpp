#include "embedded/fixture.h"

int main() {
  if (visage_smoke::sample_txt.size != 7) {
    return 1;
  }
  if (visage_smoke::second_txt.size != 7) {
    return 1;
  }
  if (visage_smoke::sample_txt.data == nullptr) {
    return 1;
  }
  if (visage_smoke::sample_txt.data[0] != 'V') {
    return 1;
  }
  if (visage_smoke::sample_txt.data[6] != '\n') {
    return 1;
  }
  if (visage_smoke::second_txt.data[0] != 's') {
    return 1;
  }
  return 0;
}
