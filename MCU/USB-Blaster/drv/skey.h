#ifndef __SKEY_H__
#define __SKEY_H__

#include "base.h"

#define SKEY_USE_SEM            (0)     // !=0使用按键互斥信号量

// 按键计时，单位ms
#define SKEY_TIME_UPDATE        (17)    // 按键状态更新间隔，需要和调用SKey_Update的周期一致
#define SKEY_TIME_ENSURE        (20)    // 按键消抖时间
#define SKEY_TIME_HOLD          (800)   // 进入长按键状态时间
#define SKEY_TIME_REPEAT        (150)   // 长按键状态后自动重复时间

// 按键状态访问操作枚举
typedef enum {
    SKEY_ACCESS_READ = 0x00,
    SKEY_ACCESS_CLEAR = 0X80,

    SKEY_ACCESS_MAX
} skey_access_t;

// 按键状态机枚举
typedef enum {
    SKEY_STATE_RELEASE_PRE,     // 按键释放消抖状态
    SKEY_STATE_RELEASE,         // 按键已释放状态
    SKEY_STATE_PRESS_PRE,       // 按键按下消抖状态
    SKEY_STATE_PRESS,           // 按键已按下状态
    SKEY_STATE_MAX
} skey_state_t;

// 按键标识位枚举
typedef enum {
    // 可外部访问的标志，获取后自动清除
    SKEY_FLAG_PRESS = 0x1,      // 按键按下标识
    SKEY_FLAG_HOLD = 0x2,       // 长按键标志
    SKEY_FLAG_REPEAT = 0x4,     // 自动重复按键标志

    // 上述标识的内部状态，只能获取不能外部清除
    SKEY_FLAG_PRESS_IN = 0x10,
    SKEY_FLAG_HOLD_IN = 0x20,
    SKEY_FLAG_REPEAT_IN = 0x40,

    SKEY_FLAG_NONE = 0x0
} skey_flag_t;

// 按键内部标识掩码
#define SKEY_FLAG_IN_MASK   0xf0

#if KEY_USE_SEM
// 按键信号量，互斥访问时使用
typedef enum {
    SKEY_SEM_USING = 0,
    SKEY_SEM_FREE  = 1
} skey_sem_t;
#endif //SKEY_USE_SEM

// 获取按键物理状态的回调函数
typedef BOOL (*skey_getk_t)(void *pdata);

// 按键计时变量类型
typedef uint16_t skey_timer_t;

// 按键数据结构体
typedef struct {
    skey_getk_t getstate;   // 获取按键状态的回调函数
    void *pdata;                // 上面函数的参数
    skey_timer_t timer;         // 按键状态计时
    skey_state_t state;         // 按键状态
    skey_flag_t flag;           // 按键标识
} skey_t;

// 初始化一个按键
// 参数：按键的结构体，状态检测函数，用户数据
void SKey_Init(skey_t *skey, skey_getk_t getstate, void *pdata);

// 使用id初始化按键数组
void SKey_InitArrayID(skey_t *skeys, uint8_t keycnt, skey_getk_t getstate, uint8_t idstart);

// 更新一个按键状态
void SKey_Update(skey_t *skey);

// 更新按键数组
void SKey_UpdateArray(skey_t *skeys, uint8_t cnt);

// 获取按键标识状态
BOOL SKey_CheckFlag(skey_t *skey, skey_flag_t flag);

#endif //__SKEY_H__
