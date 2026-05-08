#include<iostream>
#include<chrono>
#include<omp.h>

using namespace std;
using namespace chrono;

void copy(int arr[], int* temp, int size){
    for (int i = 0; i < size; i++){
        temp[i] = arr[i];
    }
}

void swap(int &a, int &b){
    int temp = a;
    a = b;
    b = temp;
}

void bubblesort(int arr[], int size){
    for (int i = 0; i < size - 1; i++){
        for (int j = 0; j < size - i - 1; j++){
            if (arr[j] > arr[j+1]){
                swap(arr[j], arr[j+1]);
            }
        }
    }
}

void odd_even_sort(int arr[], int size){
    bool sorted = false;
    while(!sorted){
        sorted = true;

        #pragma omp parallel for shared(arr, size, sorted)
        for (int i = 0; i < size - 1; i+=2){
            if (arr[i] > arr[i+1]){
                swap(arr[i], arr[i+1]);
                sorted = false;
            }
        }

        #pragma omp parallel for shared(arr, size, sorted)
        for (int i = 1; i < size - 1; i+=2){
            if (arr[i] > arr[i+1]){
                swap(arr[i], arr[i+1]);
                sorted = false;
            }
        }
    }
}

void merge(int arr[], int left, int mid, int right, int size){
    int temp[right-left+1];
    int k = 0;
    int i = left;
    int j = mid + 1;

    while (i <= mid && j <= right){
        temp[k++] = arr[i] < arr[j] ? arr[i++] : arr[j++];
    }
    while (i <= mid){
        temp[k++] = arr[i++];
    }
    while (j <= right){
        temp[k++] = arr[j++];
    }

    k = 0;
    for (int z = left; z <= right; z++){
        arr[z] = temp[k++];
    }
}

void serial_mergesort(int arr[], int size){
    serial_mergesort_helper(0, size-1, arr, size);
}

void serial_mergesort_helper(int left, int right, int arr[], int size){
    if (left < right){
        int mid = (left + right) / 2;
        serial_mergesort_helper(left, mid, arr, size);
        serial_mergesort_helper(mid+1, right, arr, size);
        merge(arr, left, mid, right, size);
    }
}

void parallel_mergesort(int arr[], int size){
    #pragma omp parallel
    {
        #pragma omp single
        {
            parallel_mergesort_helper(0, size-1, arr, size);
        }
    }
}

void parallel_mergesort_helper(int left, int right, int arr[], int size){
    if (left < right){
        int mid = (left + right) / 2;
        #pragma omp parallal sections
        {
            #pragma omp parallel section
            {
                parallel_mergesort_helper(left, mid, arr, size);
            }
            
            #pragma omp parallel section
            {
                parallel_mergesort_helper(mid+1, right, arr, size);
            }
        }
        merge(arr, left, mid, right, size);
    }
}

int main(){
    int n = 100000;
    int arr[n], temp[n];

    for (int i = 0; i < n; i++){
        arr[i] = rand() % 1000;
    }
    
    copy(arr, temp, n);
    auto start = high_resolution_clock::now();
    bubblesort(temp, n);
    auto end = high_resolution_clock::now();
    double time1 = duration<double>(end-start).count();

    copy(arr ,temp ,n);
    start = high_resolution_clock::now();
    odd_even_sort(temp, n);
    end = high_resolution_clock::now();
    double time2 = duration<double>(end-start).count();

    copy(arr ,temp ,n);
    start = high_resolution_clock::now();
    serial_mergesort(temp, n);
    end = high_resolution_clock::now();
    double time3 = duration<double>(end-start).count();

    copy(arr ,temp ,n);
    start = high_resolution_clock::now();
    parallel_mergesort(temp, n);
    end = high_resolution_clock::now();
    double time4 = duration<double>(end-start).count();

    cout << "Bubble Sort Time: " << time1 << endl;
    cout << "Odd Even Sort Time: " << time2 << endl;
    cout << "SpeedUp: " << time1 / time2 << "\n" << endl;

    cout << "Serial MergeSort Time: " << time3 << endl;
    cout << "Parallel MergeSort Time: " << time4 << endl;
    cout << "SpeedUp: " << time4 / time3 << endl;

    return 0;
}