// pi_vector_types.cl
// An implementation of the Monte Carlo pi kernel using vector data types
// instead of float/ints

#define CL_VECTOR_LENGTH (16)
#define WORK_ITEMS (4)

__kernel
__attribute__((reqd_work_group_size(WORK_ITEMS,1,1)))
void pi_async_copy(__global float *gX, __global float *gY,
    __global int * gResults, const int samples)
{
    // Workgroup copies down data using async_work_group_copy
    // Each work item computes votes for 16 elements
    const int stride = WORK_ITEMS * CL_VECTOR_LENGTH;
    int gOffset = get_group_id(0) * stride;
    int n_elements = min(stride, samples - gOffset);

    __local float lX[stride];
    __local float lY[stride];
    __local int   lResults[stride];

    // Events for burst read/write operations
    event_t read_ops[2];
    event_t write_op;

    // Perform the burst transfer.
    // Note, only the zeroth work item in the workgroup executes this code.
    async_work_group_copy(lX, &gX[gOffset], n_elements, read_ops[0]);
    async_work_group_copy(lY, &gY[gOffset], n_elements, read_ops[1]);
    wait_group_events(2, read_ops);

    // Perform the unit circle test.
    int start = get_local_id(0) * CL_VECTOR_LENGTH;
    int end   = start + CL_VECTOR_LENGTH;
    for(int i = start; i < end; i++)
    {
        float x = lX[i];
        float y = lY[i];
        lResults[i] = ((x*x + y*y) < 1.0f);
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // Write the results back to global memory.
//    printf("%i %i %i %i %i\n", get_group_id(0), get_local_id(0), samples, gOffset, n_elements);
    async_work_group_copy(&gResults[gOffset], lResults, n_elements, write_op);
    wait_group_events(1, &write_op);
}
