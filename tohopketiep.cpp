#include <bits/stdc++.h>

using namespace std;

int n, k, ok = 1;
int a[1001];

void in() {
    for (int i = 1; i <= k; i++) {
        cout << a[i] << ' ';
    }
    cout << endl;
}

void sinh() {
    int i = k;
    while (a[i] == n - k + i) { // a[i] == n - k + i là điều kiện dừng
        i--; // tìm i đầu tiên mà a[i] != n - k + i
    }
    if (i == 0) {
        ok = 0;
    } else {
        a[i]++;
        for (int j = i + 1; j <= k; j++) { // cập nhật các giá trị sau a[i]
            a[j] = a[j - 1] + 1; 
        }
    }
}

int main() {
    cin >> n >> k;
    for (int i = 1; i <= k; i++) {
        a[i] = i;
    }
    while (ok) {
        in();
        sinh();
    }
    return 0;
}