#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>

using namespace std;
using namespace chrono;

struct SortMetrics {
    long long comparisons = 0;
    long long copies = 0;
    long long duration = 0;
};

//bubble sort
void bubbleSort(vector<int>& arr, SortMetrics& metrics) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - 1 - i; ++j) {
            metrics.comparisons++;
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                metrics.copies += 3;
            }
        }
    }
}

//insertion sort
void insertionSort(vector<int>& arr, SortMetrics& metrics) {
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;
        metrics.comparisons++;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            metrics.copies++;
            j--;
            metrics.comparisons++;
        }
        arr[j + 1] = key;
        metrics.copies++;
    }
}

//quick sort
void quickSortHelper(vector<int>& arr, int low, int high, SortMetrics& metrics) {
    if (low < high) {
        int pivot = arr[high];
        int i = low - 1;
        for (int j = low; j < high; ++j) {
            metrics.comparisons++;
            if (arr[j] <= pivot) {
                i++;
                swap(arr[i], arr[j]);
                metrics.copies += 3;
            }
        }
        swap(arr[i + 1], arr[high]);
        metrics.copies += 3;
        int pi = i + 1;
        quickSortHelper(arr, low, pi - 1, metrics);
        quickSortHelper(arr, pi + 1, high, metrics);
    }
}

void quickSort(vector<int>& arr, SortMetrics& metrics) {
    quickSortHelper(arr, 0, arr.size() - 1, metrics);
}

//merge sort
void merge(vector<int>& arr, int l, int m, int r, SortMetrics& metrics) {
    int n1 = m - l + 1;
    int n2 = r - m;
    vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; ++i) {
        L[i] = arr[l + i];
        metrics.copies++;
    }
    for (int j = 0; j < n2; ++j) {
        R[j] = arr[m + 1 + j];
        metrics.copies++;
    }
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        metrics.comparisons++;
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        metrics.copies++;
        k++;
    }
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
        metrics.copies++;
    }
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
        metrics.copies++;
    }
}

void mergeSortHelper(vector<int>& arr, int l, int r, SortMetrics& metrics) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSortHelper(arr, l, m, metrics);
        mergeSortHelper(arr, m + 1, r, metrics);
        merge(arr, l, m, r, metrics);
    }
}

void mergeSort(vector<int>& arr, SortMetrics& metrics) {
    mergeSortHelper(arr, 0, arr.size() - 1, metrics);
}

//heap sort
void heapify(vector<int>& arr, int n, int i, SortMetrics& metrics) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    metrics.comparisons++;
    if (left < n && arr[left] > arr[largest]) {
        largest = left;
    }
    metrics.comparisons++;
    if (right < n && arr[right] > arr[largest]) {
        largest = right;
    }
    if (largest != i) {
        swap(arr[i], arr[largest]);
        metrics.copies += 3;
        heapify(arr, n, largest, metrics);
    }
}

void heapSort(vector<int>& arr, SortMetrics& metrics) {
    int n = arr.size();
    for (int i = n / 2 - 1; i >= 0; --i) {
        heapify(arr, n, i, metrics);
    }
    for (int i = n - 1; i >= 0; --i) {
        swap(arr[0], arr[i]);
        metrics.copies += 3;
        heapify(arr, i, 0, metrics);
    }
}

//radix sort
int getMax(const vector<int>& arr, SortMetrics& metrics) {
    int max = arr[0];
    for (int i = 1; i < arr.size(); ++i) {
        metrics.comparisons++;
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

void countSort(vector<int>& arr, int exp, SortMetrics& metrics) {
    int n = arr.size();
    vector<int> output(n);
    int count[10] = {0};
    for (int i = 0; i < n; ++i) {
        count[(arr[i] / exp) % 10]++;
    }
    for (int i = 1; i < 10; ++i) {
        count[i] += count[i - 1];
    }
    for (int i = n - 1; i >= 0; --i) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
        metrics.copies++;
    }
    for (int i = 0; i < n; ++i) {
        arr[i] = output[i];
        metrics.copies++;
    }
}

void radixSort(vector<int>& arr, SortMetrics& metrics) {
    int max = getMax(arr, metrics);
    for (int exp = 1; max / exp > 0; exp *= 10) {
        countSort(arr, exp, metrics);
    }
}

//load data from csv
vector<int> readCSV(const string& filename) {
    vector<int> data;
    ifstream file(filename);
    string line;
    int lineCount = 0;
    while (getline(file, line)) {
        lineCount++;
        if (lineCount < 6) continue;
        stringstream ss(line);
        string value;
        int colCount = 0;
        while (getline(ss, value, ',')) {
            colCount++;
            if (colCount == 2) {
                try {
                    data.push_back(stoi(value));
                } catch (const invalid_argument& e) {
                    cerr << "Invalid argument: " << value << " in line " << lineCount << endl;
                } catch (const out_of_range& e) {
                    cerr << "Out of range: " << value << " in line " << lineCount << endl;
                }
            }
        }
    }
    return data;
}

void executeSort(void(*sortFunc)(vector<int>&, SortMetrics&), vector<int> arr, const string& sortName) {
    SortMetrics metrics;
    auto start = high_resolution_clock::now();
    sortFunc(arr, metrics);
    auto end = high_resolution_clock::now();
    metrics.duration = duration_cast<microseconds>(end - start).count();
    cout << sortName << " - Comparisons: " << metrics.comparisons << ", Copies: " << metrics.copies << ", Duration: " << metrics.duration << " microseconds\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }
    string filename = argv[1];
    vector<int> data = readCSV(filename);
    if (data.empty()) {
        cerr << "No valid data read from file.\n";
        return 1;
    }

    vector<int> dataCopy;

    dataCopy = data;
    executeSort(bubbleSort, dataCopy, "Bubble Sort");

    dataCopy = data;
    executeSort(insertionSort, dataCopy, "Insertion Sort");

    dataCopy = data;
    executeSort(quickSort, dataCopy, "Quick Sort");

    dataCopy = data;
    executeSort(mergeSort, dataCopy, "Merge Sort");

    dataCopy = data;
    executeSort(heapSort, dataCopy, "Heap Sort");

    dataCopy = data;
    executeSort(radixSort, dataCopy, "Radix Sort");

    return 0;
}
