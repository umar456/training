// pi_initial.cl

__kernel
void pi_initial(__global float *gX, __global float *gY,
                __global int *results) {
  int id = get_global_id(0);

  float x = gX[id];
  float y = gY[id];

  if (x * x + y * y < 1.0f)
    results[id] = 1;
  else
    results[id] = 0;
}
