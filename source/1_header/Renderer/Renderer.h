#pragma once
#include "Application.h"

/// <summary>
/// コンパイル済み頂点シェーダー
/// </summary>
const BYTE g_FBXVS[] =
{
     68,  88,  66,  67, 102,  71,
    115, 196, 163, 187, 122,  25,
     83, 219, 103, 184, 255,  61,
    228, 230,   1,   0,   0,   0,
     56,  19,   0,   0,   5,   0,
      0,   0,  52,   0,   0,   0,
    132,   2,   0,   0, 112,   3,
      0,   0, 128,   4,   0,   0,
    156,  18,   0,   0,  82,  68,
     69,  70,  72,   2,   0,   0,
      2,   0,   0,   0, 144,   0,
      0,   0,   2,   0,   0,   0,
     60,   0,   0,   0,   0,   5,
    254, 255,   0,   1,   0,   0,
     32,   2,   0,   0,  82,  68,
     49,  49,  60,   0,   0,   0,
     24,   0,   0,   0,  32,   0,
      0,   0,  40,   0,   0,   0,
     36,   0,   0,   0,  12,   0,
      0,   0,   0,   0,   0,   0,
    124,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      1,   0,   0,   0,   1,   0,
      0,   0, 134,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   1,   0,
      0,   0,   1,   0,   0,   0,
      1,   0,   0,   0,  83,  99,
    101, 110, 101,  68,  97, 116,
     97,   0,  84, 114,  97, 110,
    115, 102, 111, 114, 109,   0,
    124,   0,   0,   0,   3,   0,
      0,   0, 192,   0,   0,   0,
    144,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
    134,   0,   0,   0,   2,   0,
      0,   0, 160,   1,   0,   0,
     64,  64,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
     56,   1,   0,   0,   0,   0,
      0,   0,  64,   0,   0,   0,
      2,   0,   0,   0,  72,   1,
      0,   0,   0,   0,   0,   0,
    255, 255, 255, 255,   0,   0,
      0,   0, 255, 255, 255, 255,
      0,   0,   0,   0, 108,   1,
      0,   0,  64,   0,   0,   0,
     64,   0,   0,   0,   2,   0,
      0,   0,  72,   1,   0,   0,
      0,   0,   0,   0, 255, 255,
    255, 255,   0,   0,   0,   0,
    255, 255, 255, 255,   0,   0,
      0,   0, 113,   1,   0,   0,
    128,   0,   0,   0,  12,   0,
      0,   0,   0,   0,   0,   0,
    124,   1,   0,   0,   0,   0,
      0,   0, 255, 255, 255, 255,
      0,   0,   0,   0, 255, 255,
    255, 255,   0,   0,   0,   0,
    118, 105, 101, 119,   0, 102,
    108, 111,  97, 116,  52, 120,
     52,   0, 171, 171,   3,   0,
      3,   0,   4,   0,   4,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
     61,   1,   0,   0, 112, 114,
    111, 106,   0, 101, 121, 101,
      0, 102, 108, 111,  97, 116,
     51,   0,   1,   0,   3,   0,
      1,   0,   3,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0, 117,   1,
      0,   0, 240,   1,   0,   0,
      0,   0,   0,   0,  64,   0,
      0,   0,   2,   0,   0,   0,
     72,   1,   0,   0,   0,   0,
      0,   0, 255, 255, 255, 255,
      0,   0,   0,   0, 255, 255,
    255, 255,   0,   0,   0,   0,
    246,   1,   0,   0,  64,   0,
      0,   0,   0,  64,   0,   0,
      2,   0,   0,   0, 252,   1,
      0,   0,   0,   0,   0,   0,
    255, 255, 255, 255,   0,   0,
      0,   0, 255, 255, 255, 255,
      0,   0,   0,   0, 119, 111,
    114, 108, 100,   0,  98, 111,
    110, 101,   0, 171,   3,   0,
      3,   0,   4,   0,   4,   0,
      0,   1,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
     61,   1,   0,   0,  77, 105,
     99, 114, 111, 115, 111, 102,
    116,  32,  40,  82,  41,  32,
     72,  76,  83,  76,  32,  83,
    104,  97, 100, 101, 114,  32,
     67, 111, 109, 112, 105, 108,
    101, 114,  32,  49,  48,  46,
     49,   0,  73,  83,  71,  78,
    228,   0,   0,   0,   7,   0,
      0,   0,   8,   0,   0,   0,
    176,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      3,   0,   0,   0,   0,   0,
      0,   0,  15,  15,   0,   0,
    185,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      3,   0,   0,   0,   1,   0,
      0,   0,  15,   0,   0,   0,
    192,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      3,   0,   0,   0,   2,   0,
      0,   0,   3,   3,   0,   0,
    201,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      3,   0,   0,   0,   3,   0,
      0,   0,   7,   0,   0,   0,
    209,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      3,   0,   0,   0,   4,   0,
      0,   0,  15,  15,   0,   0,
    215,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      1,   0,   0,   0,   5,   0,
      0,   0,  15,  15,   0,   0,
    219,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      3,   0,   0,   0,   6,   0,
      0,   0,  15,  15,   0,   0,
     80,  79,  83,  73,  84,  73,
     79,  78,   0,  78,  79,  82,
     77,  65,  76,   0,  84,  69,
     88,  67,  79,  79,  82,  68,
      0,  84,  65,  78,  71,  69,
     78,  84,   0,  67,  79,  76,
     79,  82,   0,  73,  68, 115,
      0,  87,  69,  73,  71,  72,
     84,   0, 171, 171,  79,  83,
     71,  78,   8,   1,   0,   0,
      8,   0,   0,   0,   8,   0,
      0,   0, 200,   0,   0,   0,
      0,   0,   0,   0,   1,   0,
      0,   0,   3,   0,   0,   0,
      0,   0,   0,   0,  15,   0,
      0,   0, 212,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   3,   0,   0,   0,
      1,   0,   0,   0,  15,   0,
      0,   0, 221,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   3,   0,   0,   0,
      2,   0,   0,   0,  15,  15,
      0,   0, 228,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   3,   0,   0,   0,
      3,   0,   0,   0,   3,  12,
      0,   0, 237,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   3,   0,   0,   0,
      4,   0,   0,   0,   7,  15,
      0,   0, 245,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   3,   0,   0,   0,
      5,   0,   0,   0,  15,   0,
      0,   0, 251,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   0,   0,
      6,   0,   0,   0,  15,  15,
      0,   0, 255,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   3,   0,   0,   0,
      7,   0,   0,   0,  15,  15,
      0,   0,  83,  86,  95,  80,
     79,  83,  73,  84,  73,  79,
     78,   0,  80,  79,  83,  73,
     84,  73,  79,  78,   0,  78,
     79,  82,  77,  65,  76,   0,
     84,  69,  88,  67,  79,  79,
     82,  68,   0,  84,  65,  78,
     71,  69,  78,  84,   0,  67,
     79,  76,  79,  82,   0,  73,
     68, 115,   0,  87,  69,  73,
     71,  72,  84,   0, 171, 171,
     83,  72,  69,  88,  20,  14,
      0,   0,  80,   0,   1,   0,
    133,   3,   0,   0, 106,   8,
      0,   1,  89,   0,   0,   4,
     70, 142,  32,   0,   0,   0,
      0,   0,   8,   0,   0,   0,
     89,   8,   0,   4,  70, 142,
     32,   0,   1,   0,   0,   0,
      4,   4,   0,   0,  95,   0,
      0,   3, 242,  16,  16,   0,
      0,   0,   0,   0,  95,   0,
      0,   3,  50,  16,  16,   0,
      2,   0,   0,   0,  95,   0,
      0,   3, 242,  16,  16,   0,
      4,   0,   0,   0,  95,   0,
      0,   3, 242,  16,  16,   0,
      5,   0,   0,   0,  95,   0,
      0,   3, 242,  16,  16,   0,
      6,   0,   0,   0, 103,   0,
      0,   4, 242,  32,  16,   0,
      0,   0,   0,   0,   1,   0,
      0,   0, 101,   0,   0,   3,
    242,  32,  16,   0,   1,   0,
      0,   0, 101,   0,   0,   3,
     50,  32,  16,   0,   3,   0,
      0,   0, 101,   0,   0,   3,
    242,  32,  16,   0,   5,   0,
      0,   0, 104,   0,   0,   2,
     19,   0,   0,   0,  41,   0,
      0,  10, 242,   0,  16,   0,
      0,   0,   0,   0,  70,  30,
     16,   0,   5,   0,   0,   0,
      2,  64,   0,   0,   2,   0,
      0,   0,   2,   0,   0,   0,
      2,   0,   0,   0,   2,   0,
      0,   0,  56,   0,   0,  10,
    114,   0,  16,   0,   1,   0,
      0,   0,   6,  16,  16,   0,
      6,   0,   0,   0,  38, 137,
     32,   6,   1,   0,   0,   0,
      4,   0,   0,   0,  10,   0,
     16,   0,   0,   0,   0,   0,
     54,   0,   0,   5,  18,   0,
     16,   0,   2,   0,   0,   0,
     42,   0,  16,   0,   1,   0,
      0,   0,  56,   0,   0,  10,
    114,   0,  16,   0,   3,   0,
      0,   0,   6,  16,  16,   0,
      6,   0,   0,   0,  22, 134,
     32,   6,   1,   0,   0,   0,
      5,   0,   0,   0,  10,   0,
     16,   0,   0,   0,   0,   0,
     54,   0,   0,   5,  34,   0,
     16,   0,   2,   0,   0,   0,
     10,   0,  16,   0,   3,   0,
      0,   0,  56,   0,   0,  10,
    114,   0,  16,   0,   4,   0,
      0,   0,   6,  16,  16,   0,
      6,   0,   0,   0,  70, 130,
     32,   6,   1,   0,   0,   0,
      6,   0,   0,   0,  10,   0,
     16,   0,   0,   0,   0,   0,
     54,   0,   0,   5,  66,   0,
     16,   0,   2,   0,   0,   0,
     26,   0,  16,   0,   4,   0,
      0,   0,  56,   0,   0,  10,
    114,   0,  16,   0,   5,   0,
      0,   0,   6,  16,  16,   0,
      6,   0,   0,   0,  70, 130,
     32,   6,   1,   0,   0,   0,
      7,   0,   0,   0,  10,   0,
     16,   0,   0,   0,   0,   0,
     54,   0,   0,   5, 130,   0,
     16,   0,   2,   0,   0,   0,
     26,   0,  16,   0,   5,   0,
      0,   0,  56,   0,   0,  10,
    114,   0,  16,   0,   6,   0,
      0,   0,  86,  21,  16,   0,
      6,   0,   0,   0,  38, 137,
     32,   6,   1,   0,   0,   0,
      4,   0,   0,   0,  26,   0,
     16,   0,   0,   0,   0,   0,
     54,   0,   0,   5,  18,   0,
     16,   0,   7,   0,   0,   0,
     42,   0,  16,   0,   6,   0,
      0,   0,  56,   0,   0,  10,
    114,   0,  16,   0,   8,   0,
      0,   0,  86,  21,  16,   0,
      6,   0,   0,   0,  22, 134,
     32,   6,   1,   0,   0,   0,
      5,   0,   0,   0,  26,   0,
     16,   0,   0,   0,   0,   0,
     54,   0,   0,   5,  34,   0,
     16,   0,   7,   0,   0,   0,
     10,   0,  16,   0,   8,   0,
      0,   0,  56,   0,   0,  10,
    114,   0,  16,   0,   9,   0,
      0,   0,  86,  21,  16,   0,
      6,   0,   0,   0,  70, 130,
     32,   6,   1,   0,   0,   0,
      6,   0,   0,   0,  26,   0,
     16,   0,   0,   0,   0,   0,
     54,   0,   0,   5,  66,   0,
     16,   0,   7,   0,   0,   0,
     26,   0,  16,   0,   9,   0,
      0,   0,  56,   0,   0,  10,
    114,   0,  16,   0,  10,   0,
      0,   0,  86,  21,  16,   0,
      6,   0,   0,   0,  70, 130,
     32,   6,   1,   0,   0,   0,
      7,   0,   0,   0,  26,   0,
     16,   0,   0,   0,   0,   0,
     54,   0,   0,   5, 130,   0,
     16,   0,   7,   0,   0,   0,
     26,   0,  16,   0,  10,   0,
      0,   0,   0,   0,   0,   7,
    242,   0,  16,   0,   2,   0,
      0,   0,  70,  14,  16,   0,
      2,   0,   0,   0,  70,  14,
     16,   0,   7,   0,   0,   0,
     56,   0,   0,  10, 114,   0,
     16,   0,   7,   0,   0,   0,
    166,  26,  16,   0,   6,   0,
      0,   0,  38, 137,  32,   6,
      1,   0,   0,   0,   4,   0,
      0,   0,  42,   0,  16,   0,
      0,   0,   0,   0,  54,   0,
      0,   5,  18,   0,  16,   0,
     11,   0,   0,   0,  42,   0,
     16,   0,   7,   0,   0,   0,
     56,   0,   0,  10, 114,   0,
     16,   0,  12,   0,   0,   0,
    166,  26,  16,   0,   6,   0,
      0,   0,  22, 134,  32,   6,
      1,   0,   0,   0,   5,   0,
      0,   0,  42,   0,  16,   0,
      0,   0,   0,   0,  54,   0,
      0,   5,  34,   0,  16,   0,
     11,   0,   0,   0,  10,   0,
     16,   0,  12,   0,   0,   0,
     56,   0,   0,  10, 114,   0,
     16,   0,  13,   0,   0,   0,
    166,  26,  16,   0,   6,   0,
      0,   0,  70, 130,  32,   6,
      1,   0,   0,   0,   6,   0,
      0,   0,  42,   0,  16,   0,
      0,   0,   0,   0,  54,   0,
      0,   5,  66,   0,  16,   0,
     11,   0,   0,   0,  26,   0,
     16,   0,  13,   0,   0,   0,
     56,   0,   0,  10, 114,   0,
     16,   0,  14,   0,   0,   0,
    166,  26,  16,   0,   6,   0,
      0,   0,  70, 130,  32,   6,
      1,   0,   0,   0,   7,   0,
      0,   0,  42,   0,  16,   0,
      0,   0,   0,   0,  54,   0,
      0,   5, 130,   0,  16,   0,
     11,   0,   0,   0,  26,   0,
     16,   0,  14,   0,   0,   0,
      0,   0,   0,   7, 242,   0,
     16,   0,   2,   0,   0,   0,
     70,  14,  16,   0,   2,   0,
      0,   0,  70,  14,  16,   0,
     11,   0,   0,   0,  56,   0,
      0,  10, 114,   0,  16,   0,
     11,   0,   0,   0, 246,  31,
     16,   0,   6,   0,   0,   0,
     38, 137,  32,   6,   1,   0,
      0,   0,   4,   0,   0,   0,
     58,   0,  16,   0,   0,   0,
      0,   0,  54,   0,   0,   5,
     18,   0,  16,   0,  15,   0,
      0,   0,  42,   0,  16,   0,
     11,   0,   0,   0,  56,   0,
      0,  10, 114,   0,  16,   0,
     16,   0,   0,   0, 246,  31,
     16,   0,   6,   0,   0,   0,
     22, 134,  32,   6,   1,   0,
      0,   0,   5,   0,   0,   0,
     58,   0,  16,   0,   0,   0,
      0,   0,  54,   0,   0,   5,
     34,   0,  16,   0,  15,   0,
      0,   0,  10,   0,  16,   0,
     16,   0,   0,   0,  56,   0,
      0,  10, 114,   0,  16,   0,
     17,   0,   0,   0, 246,  31,
     16,   0,   6,   0,   0,   0,
     70, 130,  32,   6,   1,   0,
      0,   0,   6,   0,   0,   0,
     58,   0,  16,   0,   0,   0,
      0,   0,  54,   0,   0,   5,
     66,   0,  16,   0,  15,   0,
      0,   0,  26,   0,  16,   0,
     17,   0,   0,   0,  56,   0,
      0,  10, 114,   0,  16,   0,
     18,   0,   0,   0, 246,  31,
     16,   0,   6,   0,   0,   0,
     70, 130,  32,   6,   1,   0,
      0,   0,   7,   0,   0,   0,
     58,   0,  16,   0,   0,   0,
      0,   0,  54,   0,   0,   5,
    130,   0,  16,   0,  15,   0,
      0,   0,  26,   0,  16,   0,
     18,   0,   0,   0,   0,   0,
      0,   7, 242,   0,  16,   0,
      2,   0,   0,   0,  70,  14,
     16,   0,   2,   0,   0,   0,
     70,  14,  16,   0,  15,   0,
      0,   0,  24,   0,   0,  10,
    242,   0,  16,   0,  15,   0,
      0,   0,  70,  14,  16,   0,
      2,   0,   0,   0,   2,  64,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
     55,   0,   0,  12, 242,   0,
     16,   0,   2,   0,   0,   0,
     70,  14,  16,   0,  15,   0,
      0,   0,   2,  64,   0,   0,
      0,   0,   0,   0,   0,   0,
    128,  63,   0,   0,   0,   0,
      0,   0,   0,   0,  70,  14,
     16,   0,   2,   0,   0,   0,
     17,   0,   0,   7,  18,   0,
     16,   0,   2,   0,   0,   0,
     70,  14,  16,   0,   2,   0,
      0,   0,  70,  30,  16,   0,
      0,   0,   0,   0,  56,   0,
      0,   8, 242,   0,  16,   0,
      2,   0,   0,   0,   6,   0,
     16,   0,   2,   0,   0,   0,
     70, 142,  32,   0,   1,   0,
      0,   0,   1,   0,   0,   0,
     54,   0,   0,   5,  18,   0,
     16,   0,   3,   0,   0,   0,
     26,   0,  16,   0,   1,   0,
      0,   0,  54,   0,   0,   5,
     34,   0,  16,   0,   1,   0,
      0,   0,  42,   0,  16,   0,
      3,   0,   0,   0,  54,   0,
      0,   5,  66,   0,  16,   0,
      3,   0,   0,   0,  10,   0,
     16,   0,   4,   0,   0,   0,
     54,   0,   0,   5,  66,   0,
     16,   0,   1,   0,   0,   0,
     42,   0,  16,   0,   4,   0,
      0,   0,  54,   0,   0,   5,
    130,   0,  16,   0,   3,   0,
      0,   0,  10,   0,  16,   0,
      5,   0,   0,   0,  54,   0,
      0,   5, 130,   0,  16,   0,
      1,   0,   0,   0,  42,   0,
     16,   0,   5,   0,   0,   0,
     54,   0,   0,   5,  18,   0,
     16,   0,   8,   0,   0,   0,
     26,   0,  16,   0,   6,   0,
      0,   0,  54,   0,   0,   5,
     34,   0,  16,   0,   6,   0,
      0,   0,  42,   0,  16,   0,
      8,   0,   0,   0,  54,   0,
      0,   5,  66,   0,  16,   0,
      8,   0,   0,   0,  10,   0,
     16,   0,   9,   0,   0,   0,
     54,   0,   0,   5,  66,   0,
     16,   0,   6,   0,   0,   0,
     42,   0,  16,   0,   9,   0,
      0,   0,  54,   0,   0,   5,
    130,   0,  16,   0,   8,   0,
      0,   0,  10,   0,  16,   0,
     10,   0,   0,   0,  54,   0,
      0,   5, 130,   0,  16,   0,
      6,   0,   0,   0,  42,   0,
     16,   0,  10,   0,   0,   0,
      0,   0,   0,   7, 242,   0,
     16,   0,   1,   0,   0,   0,
     70,  14,  16,   0,   1,   0,
      0,   0,  70,  14,  16,   0,
      6,   0,   0,   0,   0,   0,
      0,   7, 242,   0,  16,   0,
      3,   0,   0,   0,  70,  14,
     16,   0,   3,   0,   0,   0,
     70,  14,  16,   0,   8,   0,
      0,   0,  54,   0,   0,   5,
     18,   0,  16,   0,  12,   0,
      0,   0,  26,   0,  16,   0,
      7,   0,   0,   0,  54,   0,
      0,   5,  34,   0,  16,   0,
      7,   0,   0,   0,  42,   0,
     16,   0,  12,   0,   0,   0,
     54,   0,   0,   5,  66,   0,
     16,   0,  12,   0,   0,   0,
     10,   0,  16,   0,  13,   0,
      0,   0,  54,   0,   0,   5,
     66,   0,  16,   0,   7,   0,
      0,   0,  42,   0,  16,   0,
     13,   0,   0,   0,  54,   0,
      0,   5, 130,   0,  16,   0,
     12,   0,   0,   0,  10,   0,
     16,   0,  14,   0,   0,   0,
     54,   0,   0,   5, 130,   0,
     16,   0,   7,   0,   0,   0,
     42,   0,  16,   0,  14,   0,
      0,   0,   0,   0,   0,   7,
    242,   0,  16,   0,   1,   0,
      0,   0,  70,  14,  16,   0,
      1,   0,   0,   0,  70,  14,
     16,   0,   7,   0,   0,   0,
      0,   0,   0,   7, 242,   0,
     16,   0,   3,   0,   0,   0,
     70,  14,  16,   0,   3,   0,
      0,   0,  70,  14,  16,   0,
     12,   0,   0,   0,  54,   0,
      0,   5,  18,   0,  16,   0,
     16,   0,   0,   0,  26,   0,
     16,   0,  11,   0,   0,   0,
     54,   0,   0,   5,  34,   0,
     16,   0,  11,   0,   0,   0,
     42,   0,  16,   0,  16,   0,
      0,   0,  54,   0,   0,   5,
     66,   0,  16,   0,  16,   0,
      0,   0,  10,   0,  16,   0,
     17,   0,   0,   0,  54,   0,
      0,   5,  66,   0,  16,   0,
     11,   0,   0,   0,  42,   0,
     16,   0,  17,   0,   0,   0,
     54,   0,   0,   5, 130,   0,
     16,   0,  16,   0,   0,   0,
     10,   0,  16,   0,  18,   0,
      0,   0,  54,   0,   0,   5,
    130,   0,  16,   0,  11,   0,
      0,   0,  42,   0,  16,   0,
     18,   0,   0,   0,   0,   0,
      0,   7, 242,   0,  16,   0,
      1,   0,   0,   0,  70,  14,
     16,   0,   1,   0,   0,   0,
     70,  14,  16,   0,  11,   0,
      0,   0,   0,   0,   0,   7,
    242,   0,  16,   0,   3,   0,
      0,   0,  70,  14,  16,   0,
      3,   0,   0,   0,  70,  14,
     16,   0,  16,   0,   0,   0,
     24,   0,   0,  10, 242,   0,
     16,   0,   4,   0,   0,   0,
     70,  14,  16,   0,   3,   0,
      0,   0,   2,  64,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,  55,   0,
      0,  12, 242,   0,  16,   0,
      3,   0,   0,   0,  70,  14,
     16,   0,   4,   0,   0,   0,
      2,  64,   0,   0,   0,   0,
    128,  63,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,  70,  14,  16,   0,
      3,   0,   0,   0,  17,   0,
      0,   7,  18,   0,  16,   0,
      3,   0,   0,   0,  70,  14,
     16,   0,   3,   0,   0,   0,
     70,  30,  16,   0,   0,   0,
      0,   0,  50,   0,   0,  10,
    242,   0,  16,   0,   2,   0,
      0,   0,  70, 142,  32,   0,
      1,   0,   0,   0,   0,   0,
      0,   0,   6,   0,  16,   0,
      3,   0,   0,   0,  70,  14,
     16,   0,   2,   0,   0,   0,
     24,   0,   0,  10, 242,   0,
     16,   0,   3,   0,   0,   0,
     70,  14,  16,   0,   1,   0,
      0,   0,   2,  64,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,  55,   0,
      0,  12, 242,   0,  16,   0,
      1,   0,   0,   0,  70,  14,
     16,   0,   3,   0,   0,   0,
      2,  64,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0, 128,  63,   0,   0,
      0,   0,  70,  14,  16,   0,
      1,   0,   0,   0,  17,   0,
      0,   7,  18,   0,  16,   0,
      1,   0,   0,   0,  70,  14,
     16,   0,   1,   0,   0,   0,
     70,  30,  16,   0,   0,   0,
      0,   0,  50,   0,   0,  10,
    242,   0,  16,   0,   1,   0,
      0,   0,  70, 142,  32,   0,
      1,   0,   0,   0,   2,   0,
      0,   0,   6,   0,  16,   0,
      1,   0,   0,   0,  70,  14,
     16,   0,   2,   0,   0,   0,
     56,   0,   0,  10,  18,   0,
     16,   0,   2,   0,   0,   0,
     10,  16,  16,   0,   6,   0,
      0,   0,  58, 128,  32,   6,
      1,   0,   0,   0,   4,   0,
      0,   0,  10,   0,  16,   0,
      0,   0,   0,   0,  56,   0,
      0,  10,  34,   0,  16,   0,
      2,   0,   0,   0,  10,  16,
     16,   0,   6,   0,   0,   0,
     58, 128,  32,   6,   1,   0,
      0,   0,   5,   0,   0,   0,
     10,   0,  16,   0,   0,   0,
      0,   0,  56,   0,   0,  10,
     66,   0,  16,   0,   2,   0,
      0,   0,  10,  16,  16,   0,
      6,   0,   0,   0,  58, 128,
     32,   6,   1,   0,   0,   0,
      6,   0,   0,   0,  10,   0,
     16,   0,   0,   0,   0,   0,
     56,   0,   0,  10, 130,   0,
     16,   0,   2,   0,   0,   0,
     10,  16,  16,   0,   6,   0,
      0,   0,  58, 128,  32,   6,
      1,   0,   0,   0,   7,   0,
      0,   0,  10,   0,  16,   0,
      0,   0,   0,   0,  56,   0,
      0,  10,  18,   0,  16,   0,
      3,   0,   0,   0,  26,  16,
     16,   0,   6,   0,   0,   0,
     58, 128,  32,   6,   1,   0,
      0,   0,   4,   0,   0,   0,
     26,   0,  16,   0,   0,   0,
      0,   0,  56,   0,   0,  10,
     34,   0,  16,   0,   3,   0,
      0,   0,  26,  16,  16,   0,
      6,   0,   0,   0,  58, 128,
     32,   6,   1,   0,   0,   0,
      5,   0,   0,   0,  26,   0,
     16,   0,   0,   0,   0,   0,
     56,   0,   0,  10,  66,   0,
     16,   0,   3,   0,   0,   0,
     26,  16,  16,   0,   6,   0,
      0,   0,  58, 128,  32,   6,
      1,   0,   0,   0,   6,   0,
      0,   0,  26,   0,  16,   0,
      0,   0,   0,   0,  56,   0,
      0,  10, 130,   0,  16,   0,
      3,   0,   0,   0,  26,  16,
     16,   0,   6,   0,   0,   0,
     58, 128,  32,   6,   1,   0,
      0,   0,   7,   0,   0,   0,
     26,   0,  16,   0,   0,   0,
      0,   0,   0,   0,   0,   7,
    242,   0,  16,   0,   2,   0,
      0,   0,  70,  14,  16,   0,
      2,   0,   0,   0,  70,  14,
     16,   0,   3,   0,   0,   0,
     56,   0,   0,  10,  18,   0,
     16,   0,   3,   0,   0,   0,
     42,  16,  16,   0,   6,   0,
      0,   0,  58, 128,  32,   6,
      1,   0,   0,   0,   4,   0,
      0,   0,  42,   0,  16,   0,
      0,   0,   0,   0,  56,   0,
      0,  10,  34,   0,  16,   0,
      3,   0,   0,   0,  42,  16,
     16,   0,   6,   0,   0,   0,
     58, 128,  32,   6,   1,   0,
      0,   0,   5,   0,   0,   0,
     42,   0,  16,   0,   0,   0,
      0,   0,  56,   0,   0,  10,
     66,   0,  16,   0,   3,   0,
      0,   0,  42,  16,  16,   0,
      6,   0,   0,   0,  58, 128,
     32,   6,   1,   0,   0,   0,
      6,   0,   0,   0,  42,   0,
     16,   0,   0,   0,   0,   0,
     56,   0,   0,  10, 130,   0,
     16,   0,   3,   0,   0,   0,
     42,  16,  16,   0,   6,   0,
      0,   0,  58, 128,  32,   6,
      1,   0,   0,   0,   7,   0,
      0,   0,  42,   0,  16,   0,
      0,   0,   0,   0,   0,   0,
      0,   7, 242,   0,  16,   0,
      2,   0,   0,   0,  70,  14,
     16,   0,   2,   0,   0,   0,
     70,  14,  16,   0,   3,   0,
      0,   0,  56,   0,   0,  10,
     18,   0,  16,   0,   3,   0,
      0,   0,  58,  16,  16,   0,
      6,   0,   0,   0,  58, 128,
     32,   6,   1,   0,   0,   0,
      4,   0,   0,   0,  58,   0,
     16,   0,   0,   0,   0,   0,
     56,   0,   0,  10,  34,   0,
     16,   0,   3,   0,   0,   0,
     58,  16,  16,   0,   6,   0,
      0,   0,  58, 128,  32,   6,
      1,   0,   0,   0,   5,   0,
      0,   0,  58,   0,  16,   0,
      0,   0,   0,   0,  56,   0,
      0,  10,  66,   0,  16,   0,
      3,   0,   0,   0,  58,  16,
     16,   0,   6,   0,   0,   0,
     58, 128,  32,   6,   1,   0,
      0,   0,   6,   0,   0,   0,
     58,   0,  16,   0,   0,   0,
      0,   0,  56,   0,   0,  10,
    130,   0,  16,   0,   3,   0,
      0,   0,  58,  16,  16,   0,
      6,   0,   0,   0,  58, 128,
     32,   6,   1,   0,   0,   0,
      7,   0,   0,   0,  58,   0,
     16,   0,   0,   0,   0,   0,
      0,   0,   0,   7, 242,   0,
     16,   0,   0,   0,   0,   0,
     70,  14,  16,   0,   2,   0,
      0,   0,  70,  14,  16,   0,
      3,   0,   0,   0,  24,   0,
      0,  10, 242,   0,  16,   0,
      2,   0,   0,   0,  70,  14,
     16,   0,   0,   0,   0,   0,
      2,  64,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,  55,   0,   0,  12,
    242,   0,  16,   0,   0,   0,
      0,   0,  70,  14,  16,   0,
      2,   0,   0,   0,   2,  64,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0, 128,  63,
     70,  14,  16,   0,   0,   0,
      0,   0,  17,   0,   0,   7,
     18,   0,  16,   0,   0,   0,
      0,   0,  70,  14,  16,   0,
      0,   0,   0,   0,  70,  30,
     16,   0,   0,   0,   0,   0,
     50,   0,   0,  10, 242,   0,
     16,   0,   0,   0,   0,   0,
     70, 142,  32,   0,   1,   0,
      0,   0,   3,   0,   0,   0,
      6,   0,  16,   0,   0,   0,
      0,   0,  70,  14,  16,   0,
      1,   0,   0,   0,  56,   0,
      0,   8, 242,   0,  16,   0,
      1,   0,   0,   0,  86,   5,
     16,   0,   0,   0,   0,   0,
     70, 142,  32,   0,   0,   0,
      0,   0,   1,   0,   0,   0,
     50,   0,   0,  10, 242,   0,
     16,   0,   1,   0,   0,   0,
     70, 142,  32,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      6,   0,  16,   0,   0,   0,
      0,   0,  70,  14,  16,   0,
      1,   0,   0,   0,  50,   0,
      0,  10, 242,   0,  16,   0,
      1,   0,   0,   0,  70, 142,
     32,   0,   0,   0,   0,   0,
      2,   0,   0,   0, 166,  10,
     16,   0,   0,   0,   0,   0,
     70,  14,  16,   0,   1,   0,
      0,   0,  50,   0,   0,  10,
    242,   0,  16,   0,   0,   0,
      0,   0,  70, 142,  32,   0,
      0,   0,   0,   0,   3,   0,
      0,   0, 246,  15,  16,   0,
      0,   0,   0,   0,  70,  14,
     16,   0,   1,   0,   0,   0,
     56,   0,   0,   8, 242,   0,
     16,   0,   1,   0,   0,   0,
     86,   5,  16,   0,   0,   0,
      0,   0,  70, 142,  32,   0,
      0,   0,   0,   0,   5,   0,
      0,   0,  50,   0,   0,  10,
    242,   0,  16,   0,   1,   0,
      0,   0,  70, 142,  32,   0,
      0,   0,   0,   0,   4,   0,
      0,   0,   6,   0,  16,   0,
      0,   0,   0,   0,  70,  14,
     16,   0,   1,   0,   0,   0,
     50,   0,   0,  10, 242,   0,
     16,   0,   1,   0,   0,   0,
     70, 142,  32,   0,   0,   0,
      0,   0,   6,   0,   0,   0,
    166,  10,  16,   0,   0,   0,
      0,   0,  70,  14,  16,   0,
      1,   0,   0,   0,  50,   0,
      0,  10, 242,  32,  16,   0,
      0,   0,   0,   0,  70, 142,
     32,   0,   0,   0,   0,   0,
      7,   0,   0,   0, 246,  15,
     16,   0,   0,   0,   0,   0,
     70,  14,  16,   0,   1,   0,
      0,   0,  54,   0,   0,   5,
    242,  32,  16,   0,   1,   0,
      0,   0,  70,  14,  16,   0,
      0,   0,   0,   0,  54,   0,
      0,   5,  50,  32,  16,   0,
      3,   0,   0,   0,  70,  16,
     16,   0,   2,   0,   0,   0,
     54,   0,   0,   5, 242,  32,
     16,   0,   5,   0,   0,   0,
     70,  30,  16,   0,   4,   0,
      0,   0,  62,   0,   0,   1,
     83,  84,  65,  84, 148,   0,
      0,   0, 113,   0,   0,   0,
     19,   0,   0,   0,   0,   0,
      0,   0,   9,   0,   0,   0,
     64,   0,   0,   0,   1,   0,
      0,   0,   0,   0,   0,   0,
      1,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
     43,   0,   0,   0,   4,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0
};

/// <summary>
/// コンパイル済みピクセルシェーダー
/// </summary>
const BYTE g_FBXPS[] =
{
     68,  88,  66,  67, 239, 104,
    246, 167,  96,  17, 143,  71,
     60,  87,  45, 197, 191, 100,
    146, 110,   1,   0,   0,   0,
     64,   3,   0,   0,   5,   0,
      0,   0,  52,   0,   0,   0,
    232,   0,   0,   0, 248,   1,
      0,   0,  44,   2,   0,   0,
    164,   2,   0,   0,  82,  68,
     69,  70, 172,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   2,   0,   0,   0,
     60,   0,   0,   0,   0,   5,
    255, 255,   0,   1,   0,   0,
    132,   0,   0,   0,  82,  68,
     49,  49,  60,   0,   0,   0,
     24,   0,   0,   0,  32,   0,
      0,   0,  40,   0,   0,   0,
     36,   0,   0,   0,  12,   0,
      0,   0,   0,   0,   0,   0,
    124,   0,   0,   0,   3,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      1,   0,   0,   0,   1,   0,
      0,   0, 128,   0,   0,   0,
      2,   0,   0,   0,   5,   0,
      0,   0,   4,   0,   0,   0,
    255, 255, 255, 255,   0,   0,
      0,   0,   1,   0,   0,   0,
     13,   0,   0,   0, 115, 109,
    112,   0, 116, 101, 120,   0,
     77, 105,  99, 114, 111, 115,
    111, 102, 116,  32,  40,  82,
     41,  32,  72,  76,  83,  76,
     32,  83, 104,  97, 100, 101,
    114,  32,  67, 111, 109, 112,
    105, 108, 101, 114,  32,  49,
     48,  46,  49,   0,  73,  83,
     71,  78,   8,   1,   0,   0,
      8,   0,   0,   0,   8,   0,
      0,   0, 200,   0,   0,   0,
      0,   0,   0,   0,   1,   0,
      0,   0,   3,   0,   0,   0,
      0,   0,   0,   0,  15,   0,
      0,   0, 212,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   3,   0,   0,   0,
      1,   0,   0,   0,  15,   0,
      0,   0, 221,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   3,   0,   0,   0,
      2,   0,   0,   0,  15,   0,
      0,   0, 228,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   3,   0,   0,   0,
      3,   0,   0,   0,   3,   3,
      0,   0, 237,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   3,   0,   0,   0,
      4,   0,   0,   0,   7,   0,
      0,   0, 245,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   3,   0,   0,   0,
      5,   0,   0,   0,  15,   0,
      0,   0, 251,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   0,   0,
      6,   0,   0,   0,  15,   0,
      0,   0, 255,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   3,   0,   0,   0,
      7,   0,   0,   0,  15,   0,
      0,   0,  83,  86,  95,  80,
     79,  83,  73,  84,  73,  79,
     78,   0,  80,  79,  83,  73,
     84,  73,  79,  78,   0,  78,
     79,  82,  77,  65,  76,   0,
     84,  69,  88,  67,  79,  79,
     82,  68,   0,  84,  65,  78,
     71,  69,  78,  84,   0,  67,
     79,  76,  79,  82,   0,  73,
     68, 115,   0,  87,  69,  73,
     71,  72,  84,   0, 171, 171,
     79,  83,  71,  78,  44,   0,
      0,   0,   1,   0,   0,   0,
      8,   0,   0,   0,  32,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   3,   0,
      0,   0,   0,   0,   0,   0,
     15,   0,   0,   0,  83,  86,
     95,  84,  65,  82,  71,  69,
     84,   0, 171, 171,  83,  72,
     69,  88, 112,   0,   0,   0,
     80,   0,   0,   0,  28,   0,
      0,   0, 106,   8,   0,   1,
     90,   0,   0,   3,   0,  96,
     16,   0,   0,   0,   0,   0,
     88,  24,   0,   4,   0, 112,
     16,   0,   0,   0,   0,   0,
     85,  85,   0,   0,  98,  16,
      0,   3,  50,  16,  16,   0,
      3,   0,   0,   0, 101,   0,
      0,   3, 242,  32,  16,   0,
      0,   0,   0,   0,  69,   0,
      0, 139, 194,   0,   0, 128,
     67,  85,  21,   0, 242,  32,
     16,   0,   0,   0,   0,   0,
     70,  16,  16,   0,   3,   0,
      0,   0,  70, 126,  16,   0,
      0,   0,   0,   0,   0,  96,
     16,   0,   0,   0,   0,   0,
     62,   0,   0,   1,  83,  84,
     65,  84, 148,   0,   0,   0,
      2,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      2,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   1,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   0,   0
};

/// <summary>
/// パイプラインステートやルートシグネチャ等の描画処理に必要なオブジェクトを管理するクラス
/// </summary>
class Dx12Wrapper;
class Renderer
{
	template<typename T>
	using ComPtr = ComPtr<T>;

private:
	SIZE _winSize;												//ウィンドウサイズ

	ComPtr<ID3D12PipelineState> _pipeline;						//パイプラインステート（パイプライン設定を定義するオブジェクト）
	ComPtr<ID3D12RootSignature> _rootSignature;					//ルートシグネチャ（スロットと各ビューが管理するバッファーを関連付ける）
    
    //コンストラクタ
    Renderer();
    Renderer(const Renderer&) = delete;
    //デストラクタ
    ~Renderer();

	HRESULT CreateRootSignature();								//ルートシグネチャ初期化関数
	HRESULT CreateGraphicsPipelineForPMD();						//パイプラインステート初期化関数
	bool CheckCompilerResult(HRESULT result, ID3DBlob* error);	//シェーダー読み込みの成否を確認する関数

public:
    //シングルトンを返す
    static Renderer& Instance();

    //初期化関数
    void Init();

	ID3D12PipelineState* GetPipelineState()const;	//パイプラインステートを返す関数
	ID3D12RootSignature* GetRootSignature()const;	//ルートシグネチャを返す関数
};