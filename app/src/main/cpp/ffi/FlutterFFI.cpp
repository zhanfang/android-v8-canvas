//
// Created by zhanfang on 2021/4/27.
//

extern "C" {
    // __attribute__((visibility("default"))) __attribute__((used))
    int native_add(int x, int y) { return x + y; }

    double double_add(double x, double y) { return x + y; }
}

