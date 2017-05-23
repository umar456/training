// pi_vector_types.cl
// An implementation of the Monte Carlo pi kernel using vector data types
// instead of float/ints

__kernel
void pi_vector_with_reduction(__global float16 *gX, __global float16 *gY,
                         __global int *results) {
  int id = get_global_id(0);

  float16 x = gX[id];
  float16 y = gY[id];

  // Note, this logical operation will return 0 if false and
  // -1 (all bits set) if true.
  int16 in_circle = ((x * x + y * y) < 1.0f) * -1;

  int sum = 0;
  for (int i = 0; i < 16; i++)
    sum += in_circle[i];

  results[id] = sum;
}
