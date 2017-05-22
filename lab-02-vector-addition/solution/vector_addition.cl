__kernel void add(__global int* a, 
	__global int* b, 
	__global int* c)
{	
	int id = get_local_id(0);
	c[id] = a[id] + b[id];
}

