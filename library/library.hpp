#include<bits/stdc++.h>
using namespace std;

class Array {
private:
    int length;
    int* array_ptr;

public:
    #pragma region //=======コンストラクタ============================================================
    Array(const Array& arr)
        : length(arr.length),
            array_ptr(arr.array_ptr == nullptr ? nullptr : new int[length]) {

        for(int i=0; i<length; i++){
            array_ptr[i]=arr.array_ptr[i];
        }
    }
    Array(Array&& arr)
        : length(arr.length),
            array_ptr(arr.array_ptr){
        
        arr.length = 0;
        arr.array_ptr = nullptr;
    }
    Array(int n)
        : length(n),
            array_ptr(new int[n]){
    }
    Array()
        : length(),
            array_ptr(nullptr){
    }
    #pragma endregion //====コンストラクタ==========================================================

    //デストラクタ
    ~Array() noexcept {
        delete[] array_ptr;
    }

    //size取得
    int size() const { return length; }

    #pragma region //=======メモリ管理==============================================================
    //メモリ確保
    void allocate(int len) {
        delete[] array_ptr;
        length = len;
        array_ptr = (length == 0 ? nullptr : new int[length]);
    }
    //メモリ解放
    void clear() {
        delete[] array_ptr;
        length = 0;
        array_ptr = nullptr;
    }
    #pragma endregion //====メモリ管理==============================================================

    #pragma region //=======要素アクセス=============================================================
    //読み取りのみ
    int at(int n) const {
        assert(0 <= n && n < length);
        return array_ptr[n];           
    }
    //読み書き
    int& at(int n) {
        assert(0 <= n && n < length);
        return array_ptr[n];
    }
    //読み取りのみ
    int operator[](int n) const {
        assert(0 <= n && n < length);
        return array_ptr[n];     
    }
    //読み書き
    int& operator[](int n) {
        assert(0 <= n && n < length);
        return array_ptr[n];     
    }
    #pragma endregion //====要素アクセス=============================================================

    #pragma region //=======代入演算子==============================================================
    //左辺値を代入
    Array& operator=(const Array& arr){
        if(this == &arr){ return *this; }

        allocate(arr.length);
        for(int i=0; i<length; i++){
            array_ptr[i]= arr.array_ptr[i];
        }

        return *this;
    }
    //右辺値を代入
    Array& operator=(Array&& arr){
        if(this == &arr){ return *this; }
        delete[] array_ptr;
        length = arr.length;
        array_ptr = arr.array_ptr;
        arr.length = 0;
        arr.array_ptr = nullptr;
        
        return *this;
    }
    #pragma endregion //====代入演算子==============================================================
};