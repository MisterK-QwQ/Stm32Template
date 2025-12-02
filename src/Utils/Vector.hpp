#pragma once
#include <math.h>  // 依赖 sqrtf、fabsf（若单片机不支持硬件浮点，需用软件模拟）

// 2D向量结构体
typedef struct {
    float x;
    float y;
} Vec2;

// 3D向量结构体
typedef struct {
    float x;
    float y;
    float z;
} Vec3;

// -------------------------- Vec2 基础运算 --------------------------
// 向量加法：a + b
static inline Vec2 Vec2_Add(Vec2 a, Vec2 b) {
    Vec2 res = {a.x + b.x, a.y + b.y};
    return res;
}

// 向量减法：a - b
static inline Vec2 Vec2_Sub(Vec2 a, Vec2 b) {
    Vec2 res = {a.x - b.x, a.y - b.y};
    return res;
}

// 向量数乘：a * k
static inline Vec2 Vec2_Mul(Vec2 a, float k) {
    Vec2 res = {a.x * k, a.y * k};
    return res;
}

// 向量数除：a / k（k≠0）
static inline Vec2 Vec2_Div(Vec2 a, float k) {
    Vec2 res = {a.x / k, a.y / k};
    return res;
}

// 向量点积：a · b
static inline float Vec2_Dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

// 向量模长：|a|
static inline float Vec2_Length(Vec2 a) {
    return sqrtf(a.x * a.x + a.y * a.y);
}

// 向量归一化（单位向量）
static inline Vec2 Vec2_Normalize(Vec2 a) {
    float len = Vec2_Length(a);
    if (len < 1e-6f) return a;  // 避免除零
    return Vec2_Div(a, len);
}

// 两个向量之间的距离
static inline float Vec2_Distance(Vec2 a, Vec2 b) {
    return Vec2_Length(Vec2_Sub(a, b));
}

// 向量投影：将a投影到b上
static inline Vec2 Vec2_Project(Vec2 a, Vec2 b) {
    float b_len = Vec2_Length(b);
    if (b_len < 1e-6f) return (Vec2){0, 0};
    float dot = Vec2_Dot(a, b);
    return Vec2_Mul(b, dot / (b_len * b_len));
}

// -------------------------- Vec3 基础运算 --------------------------
// 向量加法：a + b
static inline Vec3 Vec3_Add(Vec3 a, Vec3 b) {
    Vec3 res = {a.x + b.x, a.y + b.y, a.z + b.z};
    return res;
}

// 向量减法：a - b
static inline Vec3 Vec3_Sub(Vec3 a, Vec3 b) {
    Vec3 res = {a.x - b.x, a.y - b.y, a.z - b.z};
    return res;
}

// 向量数乘：a * k
static inline Vec3 Vec3_Mul(Vec3 a, float k) {
    Vec3 res = {a.x * k, a.y * k, a.z * k};
    return res;
}

// 向量数除：a / k（k≠0）
static inline Vec3 Vec3_Div(Vec3 a, float k) {
    Vec3 res = {a.x / k, a.y / k, a.z / k};
    return res;
}

// 向量点积：a · b
static inline float Vec3_Dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// 向量叉积：a × b（仅3D支持）
static inline Vec3 Vec3_Cross(Vec3 a, Vec3 b) {
    Vec3 res = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return res;
}

// 向量模长：|a|
static inline float Vec3_Length(Vec3 a) {
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

// 向量归一化（单位向量）
static inline Vec3 Vec3_Normalize(Vec3 a) {
    float len = Vec3_Length(a);
    if (len < 1e-6f) return a;  // 避免除零
    return Vec3_Div(a, len);
}

// 两个向量之间的距离
static inline float Vec3_Distance(Vec3 a, Vec3 b) {
    return Vec3_Length(Vec3_Sub(a, b));
}

// 向量投影：将a投影到b上
static inline Vec3 Vec3_Project(Vec3 a, Vec3 b) {
    float b_len = Vec3_Length(b);
    if (b_len < 1e-6f) return (Vec3){0, 0, 0};
    float dot = Vec3_Dot(a, b);
    return Vec3_Mul(b, dot / (b_len * b_len));
}

// -------------------------- 常用常量向量 --------------------------
#define Vec2_Zero    ((Vec2){0.0f, 0.0f})
#define Vec2_One     ((Vec2){1.0f, 1.0f})
#define Vec2_Up      ((Vec2){0.0f, 1.0f})
#define Vec2_Down    ((Vec2){0.0f, -1.0f})
#define Vec2_Left    ((Vec2){-1.0f, 0.0f})
#define Vec2_Right   ((Vec2){1.0f, 0.0f})

#define Vec3_Zero    ((Vec3){0.0f, 0.0f, 0.0f})
#define Vec3_One     ((Vec3){1.0f, 1.0f, 1.0f})
#define Vec3_Up      ((Vec3){0.0f, 1.0f, 0.0f})
#define Vec3_Down    ((Vec3){0.0f, -1.0f, 0.0f})
#define Vec3_Left    ((Vec3){-1.0f, 0.0f, 0.0f})
#define Vec3_Right   ((Vec3){1.0f, 0.0f, 0.0f})
#define Vec3_Forward ((Vec3){0.0f, 0.0f, 1.0f})
#define Vec3_Back    ((Vec3){0.0f, 0.0f, -1.0f})
