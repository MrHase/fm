#include "rnd.h"

#include <random>
using namespace std;

RND::RND()
{
}
int GetRND(int s, int e)
{
    static    std::mt19937 rnd;
    static uint32_t seed_val;

    std::uniform_int_distribution<uint32_t> uint_dist10(s,e);
    uint test=uint_dist10(rnd);
    return test;
}
