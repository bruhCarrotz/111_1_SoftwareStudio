// Matrix Multiplication Kernel
// First Matrix: X * Y
// Second Matrix: Y * Z

__kernel void
matrixMul(__global float* ans, __global float* A, __global float* B, int X, int Y, int Z)
{
  
   int x_axis = get_global_id(0); 
   int y_axis = get_global_id(1);
 
   float value = 0;
   for (int i = 0; i < Y; ++i){
      value += A[y_axis * Y + i] * B[i * Z + x_axis];
   }

   ans[y_axis * Y + x_axis] = value;
}