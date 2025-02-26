/*Các thuật toán tìm kiếm*/

/*
1. Tìm kiếm tuyến tính

2. Tìm kiếm nhị phân

3. Tìm kiếm nội suy
- Tìm kiếm nội suy sử dụng công thức sau:
  mid = low + (high - low) * (key - arr[low]) / (arr[high] - arr[low])
- Tìm kiếm nội suy chỉ hiệu quả khi dãy số đã được sắp xếp.
- Độ phức tạp: O(log(log(n)))
- Ưu điểm: Hiệu quả hơn tìm kiếm nhị phân khi dãy số gần đều.
- Mục tiêu: Tìm kiếm một phần tử trong một dãy số đã sắp xếp.
Tìm kiến nội suy dựa trên việc tính toán gần đúng vị trí của phần tử cần tìm trong dãy số đã sắp xếp.

### Các bước thực hiện tìm kiếm nội suy:
1. Tính toán vị trí gần đúng của phần tử cần tìm.
2. So sánh phần tử cần tìm với phần tử ở vị trí gần đúng.
3. Nếu phần tử cần tìm bằng phần tử ở vị trí gần đúng, trả về vị trí đó.
4. Nếu phần tử cần tìm lớn hơn phần tử ở vị trí gần đúng, tìm kiếm phần tử cần tìm trong nửa phía bên phải của dãy số.
5. Nếu phần tử cần tìm nhỏ hơn phần tử ở vị trí gần đúng, tìm kiếm phần tử cần tìm trong nửa phía bên trái của dãy số.
6. Lặp lại các bước trên cho đến khi tìm thấy phần tử cần tìm hoặc không tìm thấy.

So sánh và cập nhật
- So sánh phần tử cần tìm với phần tử ở vị trí gần đúng.
- Nếu phần tử cần tìm bằng phần tử ở vị trí gần đúng, trả về vị trí đó.
- Nếu phần tử cần tìm lớn hơn phần tử ở vị trí gần đúng, tìm kiếm phần tử cần tìm trong nửa phía bên phải của dãy số.
- Nếu phần tử cần tìm nhỏ hơn phần tử ở vị trí gần đúng, tìm kiếm phần tử cần tìm trong nửa phía bên trái của dãy số.

4. Tìm kiếm nhị phân nhiều chiều
- Tìm kiếm nhị phân nhiều chiều sử dụng công thức sau:
  mid = low + (high - low) / 2

*/

/* Bài tập */
/*Cho một mảng số nguyên đã sắp xếp tăng dần
Viết hàm interpolationSearch để tìm kiếm vị trí 1 phần tử trong mảng
Nếu không tìm thấy thì trả về no */

#include <bits/stdc++.h>

using namespace std;

int n, x;
int a[1001];

void in (){
    for (int i = 0; i < n; i++){
        cin >> a[i];
    }
    cout << endl;
}

void interpolationSearch(){
    int low = 0, high = n - 1; // low: vị trí đầu, high: vị trí cuối
    while (low <= high && x >= a[low] && x <= a[high]){ // x nằm trong khoảng từ a[low] đến a[high]
        if (low == high){ // nếu chỉ còn 1 phần tử
            if (a[low] == x){ // nếu phần tử đó bằng x
                cout << low << endl; // in ra vị trí
                return; // kết thúc
            }
            cout << "no" << endl; // không tìm thấy
            return;
        }
        int pos = low + (high - low) * (x - a[low]) / (a[high] - a[low]); // tìm vị trí gần đúng của x
        if (a[pos] == x){ // nếu phần tử tại vị trí pos bằng x
            cout << pos << endl; // in ra vị trí
            return; // kết thúc
        }
        if (a[pos] == x){ // nếu phần tử tại vị trí pos bằng x
            cout << pos << endl; // in ra vị trí
            return; // kết thúc
        }
        if (a[pos] < x){
            low = pos + 1;
        }
        else {
            high = pos - 1;
        }
    }
    cout << "no";
}

int main (){
    int t;
    cin >> t;
    while (t--){
        cin >> n >> x;
        in ();
        interpolationSearch();
    }
    return 0;
}