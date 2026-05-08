#include <iostream>
#include <chrono>
#include <cuda_runtime.h>

using namespace std;
using namespace chrono;

__global__ void GPU_Matrix_Multiplication(int *a, int *b, int *c, int n)
{
    int row = blockDim.y * blockIdx.y + threadIdx.y;
    int col = blockDim.x * blockIdx.x + threadIdx.x;

    if (row < n && col < n)
    {
        int val = 0;
        for (int i = 0; i < n; i++)
        {
            val += a[row * n + i] * b[i * n + col];
        }
        c[row * n + col] = val;
    }
}

void CPU_Matrix_Multiplication(int *a, int *b, int *c, int n)
{
    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            int val = 0;
            for (int i = 0; i < n; i++)
            {
                val += a[row * n + i] * b[i * n + col];
            }
            c[row * n + col] = val;
        }
    }
}

bool same(int *a, int *b, int n)
{
    for (int i = 0; i < n * n; i++)
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
    int n = 1000;
    int size = n * n * sizeof(int);

    int *c_a, *c_b, *c_c, *c_d;
    int *g_a, *g_b, *g_c;

    c_a = (int *)malloc(size);
    c_b = (int *)malloc(size);
    c_c = (int *)malloc(size);
    c_d = (int *)malloc(size);

    for (int i = 0; i < n * n; i++)
    {
        c_a[i] = rand() % 100;
        c_b[i] = rand() % 100;
    }

    cudaMalloc((void **)&g_a, size);
    cudaMalloc((void **)&g_b, size);
    cudaMalloc((void **)&g_c, size);

    int threads = 256;
    int blocks = (n + threads - 1) / threads;
    dim3 threadsPerBlock(threads, threads);
    dim3 blocksPerGrid(blocks, blocks);

    auto start = high_resolution_clock::now();
    CPU_Matrix_Multiplication(c_a, c_b, c_c, n);
    auto end = high_resolution_clock::now();
    double time1 = duration<double>(end - start).count();

    start = high_resolution_clock::now();

    cudaMemcpy(g_a, c_a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(g_b, c_b, size, cudaMemcpyHostToDevice);

    auto start_gpu = high_resolution_clock::now();
    GPU_Matrix_Multiplication<<<blocksPerGrid, threadsPerBlock>>>(g_a, g_b, g_c, n);
    cudaDeviceSynchronize();
    auto end_gpu = high_resolution_clock::now();

    cudaMemcpy(c_d, g_c, size, cudaMemcpyDeviceToHost);

    end = high_resolution_clock::now();
    double time2 = duration<double>(end_gpu - start_gpu).count();
    double time3 = duration<double>(end - start).count();

    cout << "Are the Result Matrices same? " << (same(c_c, c_d, n) ? "Yes" : "No") << endl;
    cout << "CPU Matrix Multiplication Time: " << time1 << endl;
    cout << "GPU Matrix Multiplication Time(Without Transfer Time): " << time2 << endl;
    cout << "GPU Matrix Multiplication Time(With Transfer Time): " << time3 << endl;

    cudaFree(g_a);
    cudaFree(g_b);
    cudaFree(g_c);

    free(c_a);
    free(c_b);
    free(c_c);
    free(c_d);

    return 0;
}