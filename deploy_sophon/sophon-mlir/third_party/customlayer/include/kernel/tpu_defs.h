#ifndef _TPU_DEFS_
#define _TPU_DEFS_
typedef unsigned int local_addr_t;
typedef unsigned int static_addr_t;
typedef unsigned long long l2_sram_addr_t;
typedef unsigned long long system_addr_t;
typedef unsigned long long global_addr_t;
typedef unsigned long long addr_t;
typedef unsigned long long u64;
typedef long long i64;
typedef unsigned int u32;
#define NO_USE 0

#define TENSOR_N_DIM 0
#define TENSOR_C_DIM 1
#define TENSOR_H_DIM 2
#define TENSOR_W_DIM 3

typedef enum {
    DT_INT8   = (0 << 1) | 1,
    DT_UINT8  = (0 << 1) | 0,
    DT_INT16  = (3 << 1) | 1,
    DT_UINT16 = (3 << 1) | 0,
    DT_FP16   = (1 << 1) | 1,
    DT_BFP16  = (5 << 1) | 1,
    DT_INT32  = (4 << 1) | 1,
    DT_UINT32 = (4 << 1) | 0,
    DT_FP32   = (2 << 1) | 1,
    DT_INT4   = (6 << 1) | 1,
    DT_UINT4  = (6 << 1) | 0
} data_type_t;
typedef enum {
    RM_HALF_TO_EVEN        = 0,
    RM_HALF_AWAY_FROM_ZERO = 1,
    RM_TOWARDS_ZERO        = 2,
    RM_DOWN                = 3,   /* FLOOR */
    RM_UP                  = 4,   /* CEIL */
    RM_HALF_UP             = 5,
    RM_HALF_DOWN           = 6
} rounding_mode_t;

typedef enum {
  REDUCE_MEAN = 0,
  REDUCE_SUM  = 1,
  REDUCE_MAX  = 2,
  REDUCE_MIN  = 3,
  REDUCE_PROD = 4,
  REDUCE_L2   = 5,
  REDUCE_L1   = 6,
} reduce_method_t;

typedef struct {
    int n, c, d, h, w;
} dim5;
typedef struct {
    int n, c, h, w;
} dim4;
typedef struct {
    int h, w;
} dim2;
typedef struct {
    int top, bottom, left, right;
} padding_t;
typedef struct {
    int start, end;
} range_t;
typedef union {
    unsigned short bits;
    struct {
        unsigned short frac : 10; // mantissa
        unsigned short exp  : 5;  // exponent
        unsigned short sign : 1;  // sign
    } format;
} float16;
typedef union {
    unsigned short bits;
    struct {
        unsigned short frac : 7;  // mantissa
        unsigned short exp  : 8;  // exponent
        unsigned short sign : 1;  // sign
    } format;
} bfloat16;
typedef union {
    char           s4;
    unsigned char  u4;
    char           s8;
    unsigned char  u8;
    short          s16;
    unsigned short u16;
    float16        f16;
    bfloat16       bf16;
    int            s32;
    unsigned int   u32;
    float          f32;
} scalar_t;

#endif /* _OK_DEFS_ */
