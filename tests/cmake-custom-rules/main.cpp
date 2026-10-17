#include "embedded/fixture.h"

int main()
{
    return (visage_smoke::sample_txt.size == 6 &&
            visage_smoke::sample_txt.data != nullptr &&
            visage_smoke::sample_txt.data[0] == 'V' &&
            visage_smoke::sample_txt.data[5] == '\\n') ? 0 : 1;
}
