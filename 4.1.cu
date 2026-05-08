#include <iostream>
#include <chrono>
#include <cuda_runtime.h>

using namespace std;
using namespace chrono;

__global__ void GPU_Vector_Addition(int *a, int *b, int *c, int n)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < n)
    {
        c[i] = a[i] + b[i];
    }
}

void CPU_Vector_Addition(int *a, int *b, int *c, int n)
{
    for (int i = 0; i < n; i++)
    {
        c[i] = a[i] + b[i];
    }
}

bool same(int *a, int *b, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}

int main()
{
    int n = 1000000;
    int size = n * sizeof(int);

    int *c_a, *c_b, *c_c, *c_d;
    int *g_a, *g_b, *g_c;

    c_a = (int *)malloc(size);
    c_b = (int *)malloc(size);
    c_c = (int *)malloc(size);
    c_d = (int *)malloc(size);

    for (int i = 0; i < n; i++)
    {
        c_a[i] = rand() % 1000;
        c_b[i] = rand() % 1000;
    }

    cudaMalloc((void **)&g_a, size);
    cudaMalloc((void **)&g_b, size);
    cudaMalloc((void **)&g_c, size);

    auto start = high_resolution_clock::now();
    CPU_Vector_Addition(c_a, c_b, c_c, n);
    auto end = high_resolution_clock::now();
    double time1 = duration<double>(end - start).count();

    start = high_resolution_clock::now();

    cudaMemcpy(g_a, c_a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(g_b, c_b, size, cudaMemcpyHostToDevice);

    int threadsPerBlock = 256;
    int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;

    auto start_gpu = high_resolution_clock::now();
    GPU_Vector_Addition<<<blocksPerGrid, threadsPerBlock>>>(g_a, g_b, g_c, n);
    cudaDeviceSynchronize();
    auto end_gpu = high_resolution_clock::now();

    cudaMemcpy(c_d, g_c, size, cudaMemcpyDeviceToHost);

    end = high_resolution_clock::now();

    cout << "Are the results same? : " << (same(c_c, c_d, n) ? "yes" : "no") << endl;
    cout << "Cpu time taken: " << time1 << endl;
    cout << "Gpu time taken(without transfer time): " << duration<double>(end_gpu - start_gpu).count() << endl;
    cout << "Gpu time taken(with transfer time): " << duration<double>(end - start).count() << endl;

    cudaFree(g_a);
    cudaFree(g_b);
    cudaFree(g_c);

    free(c_a);
    free(c_b);
    free(c_c);
    free(c_d);

    return 0;
}