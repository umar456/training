//increment.cl

__kernel void increment(__global int *value)
{
    *value += 2;
}
