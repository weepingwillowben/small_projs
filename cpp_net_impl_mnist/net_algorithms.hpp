
/*
 *
float activ_fn(float val){
    return 1 / (1 + exp(-val)) + 0.05*val*(val > 0);
}
float activ_fn_deriv(float val){
    float sig_val = activ_fn(val);
    return sig_val *(1-sig_val) + 0.05*(val > 0);
}*/
#include "globals.h"
using u32 = unsigned int;
using block_vec = fvec<fvec8,BATCH_SIZE/8>;
static inline float
fastpow2 (float p)
{
  float z = p - (int(p) + (p < 0));
  union { u32 i; float f; } v = { u32 ( (1 << 23) * (p + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) ) };

  return v.f;
}
static inline float
fastexp (float p)
{
  return fastpow2 (1.442695040f * p);
}
/*
static inline block_vec
fastpow2 (block_vec p)
{
  float z = p - (int(p) + (p < 0));
  union { u32 i; float f; } v = { u32 ( (1 << 23) * (p + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) ) };

  return v.f;
}
static inline block_vec
fastexp (block_vec p)
{
  return fastpow2 (p * fvec8(1.442695040f));
}
*/

float activ_fn(float val){
    return 1 / (1 + fastexp(-val));
}
float activ_fn_deriv(float val){
    //float expval = exp(-val);
    //return -(1 / sqare(1 + expval)) * expval;
    float sig_val = activ_fn(val);
    return sig_val *(1-sig_val);
}
block_vec activ_fn(block_vec val){
    for (float & f : val)
        f = activ_fn(f);
    return val;
}
block_vec activ_fn_deriv(block_vec val){
    //float expval = exp(-val);
    //return -(1 / sqare(1 + expval)) * expval;
    for (float & f : val){
        float sig_val = activ_fn(f);
        f = sig_val *(1-sig_val);
    }
    return val;
}
#define QUADRATI
#ifdef QUADRATIC
float cost(float predicted,float actual){
    return 0.5 * sqare(actual - predicted);
}
float cost_deriv(float predicted,float actual){
    return (predicted - actual);
}
#else
float cost(float predicted,float actual){
    return -(actual * log(predicted) + (1-actual) * log(1-predicted));
}
float cost_deriv(float predicted,float actual){
    return (1-actual)/(1-predicted) - actual / predicted;
}
#endif
