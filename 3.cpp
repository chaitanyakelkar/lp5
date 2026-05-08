#include<iostream>
#include<chrono>
#include<omp.h>

using namespace std;
using namespace chrono;

int serial_sum(int arr[], int n){
    int sum = 0;
    for (int i = 0; i < n; i++){
        sum += arr[i];
    }
    return sum;
}

int parallel_sum(int arr[], int n){
    int sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++){
        sum += arr[i];
    }
    return sum;
}

int serial_average(int arr[], int n){
    int sum = 0;
    for (int i = 0; i < n; i++){
        sum += arr[i];
    }
    return (float)sum / n;
}

int parallel_average(int arr[], int n){
    int sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++){
        sum += arr[i];
    }
    return (float)sum / n;
}

int serial_min(int arr[], int n){
    int min_val = arr[0];
    for(int i = 1; i < n; i++){
        if (min_val > arr[i]){
            min_val = arr[i];
        }
    }
    return min_val;
}

int parallel_min(int arr[], int n){
    int min_val = arr[0];
    #pragma omp parallel for reduction(min:min_val)
    for(int i = 1; i < n; i++){
        if (min_val > arr[i]){
            min_val = arr[i];
        }
    }
    return min_val;
}

int serial_max(int arr[], int n){
    int max_val = arr[0];
    for (int i = 1; i < n; i++){
        if (max_val < arr[i]){
            max_val = arr[i];
        }
    }
    return max_val;
}

int parallel_max(int arr[], int n){
    int max_val = arr[0];
    #pragma omp parallel for reduction(max:max_val)
    for (int i = 1; i < n; i++){
        if (max_val < arr[i]){
            max_val = arr[i];
        }
    }
}

int main(){
    int n = 100000000;
    int arr[n];

    for (int i = 0; i < n; i++){
        arr[i] = rand() % 1000;
    }

    auto start = high_resolution_clock::now();
    cout << "Serial Sum: " << serial_sum(arr, n) << endl;
    auto end = high_resolution_clock::now();
    double time1 = duration<double>(end-start).count();
    cout << "Serial Sum Time: " << time1 << endl;

    start = high_resolution_clock::now();
    cout << "Parallel Sum: " << parallel_sum(arr, n) << endl;
    end = high_resolution_clock::now();
    double time2 = duration<double>(end-start).count();
    cout << "Parallel Sum Time: " << time2 << endl;
    cout << "Sum SpeedUp: " << time1 / time2 << "\n\n" << endl;

    start = high_resolution_clock::now();
    cout << "Serial Average: " << serial_average(arr, n) << endl;
    end = high_resolution_clock::now();
    double time3 = duration<double>(end-start).count();
    cout << "Serial Average Time: " << time3 << endl;

    start = high_resolution_clock::now();
    cout << "Parallel Average: " << parallel_average(arr, n) << endl;
    end = high_resolution_clock::now();
    double time4 = duration<double>(end-start).count();
    cout << "Parallel Average Time: " << time4 << endl;
    cout << "Average SpeedUp: " << time3 / time4 << "\n\n" << endl;

    start = high_resolution_clock::now();
    cout << "Serial Min: " << serial_min(arr, n) << endl;
    end = high_resolution_clock::now();
    double time5 = duration<double>(end-start).count();
    cout << "Serial Min Time: " << time5 << endl;

    start = high_resolution_clock::now();
    cout << "Parallel Min: " << parallel_min(arr, n) << endl;
    end = high_resolution_clock::now();
    double time6 = duration<double>(end-start).count();
    cout << "Parallel Min Time: " << time6 << endl;
    cout << "Min SpeedUp: " << time5 / time6 << "\n\n" << endl;

    start = high_resolution_clock::now();
    cout << "Serial Max: " << serial_max(arr, n) << endl;
    end = high_resolution_clock::now();
    double time7 = duration<double>(end-start).count();
    cout << "Serial Max Time: " << time7 << endl;

    start = high_resolution_clock::now();
    cout << "Parallel Max: " << parallel_max(arr, n) << endl;
    end = high_resolution_clock::now();
    double time8 = duration<double>(end-start).count();
    cout << "Parallel Max Time: " << time8 << endl;
    cout << "Max SpeedUp: " << time7 / time8 << "\n\n" << endl;

    return 0;
}