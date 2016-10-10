/* 通用单按键驱动
 * eleqian 2014-10-2
 */

#include "base.h"
#include "skey.h"

/*
说明：初始化一个按键对象
参数：skey：指向按键对象的指针
      getstate：状态检测函数指针
      pdata：状态检测函数参数
*/
void SKey_Init(skey_t *skey, skey_getk_t getstate, void *pdata)
{
    skey->getstate = getstate;
    skey->pdata = pdata;
    skey->state = SKEY_STATE_RELEASE;
    skey->flag = SKEY_FLAG_NONE;
#if SKEY_USE_SEM
    skey->sem = SKEY_SEM_FREE;
#endif //SKEY_USE_SEM
    skey->timer = 0;        //用于固定周期调用状态更新函数的计时
}

// 使用id初始化按键数组
void SKey_InitArrayID(skey_t *skeys, uint8_t keycnt, skey_getk_t getstate, uint8_t idstart)
{
    uint8_t i;

    for (i = 0; i < keycnt; i++) {
        SKey_Init(skeys + i, getstate, (void *)(idstart + i));
    }
}

/*
说明：更新按键状态
要求：调用频率满足按键更新最快频率，一般要求调用周期小于50ms
*/
void SKey_Update(skey_t *skey)
{
#if SKEY_USE_SEM
    if (SKEY_SEM_FREE == skey->sem) {
        skey->sem = SKEY_SEM_USING;
#endif //SKEY_USE_SEM

        switch (skey->state) {
        case SKEY_STATE_RELEASE:
            if (skey->getstate(skey->pdata)) {
                skey->timer = 0;
                skey->flag = SKEY_FLAG_NONE;
                skey->state = SKEY_STATE_PRESS_PRE;
            }
            break;
        case SKEY_STATE_PRESS_PRE:
            skey->timer += SKEY_TIME_UPDATE;
            if (skey->timer >= SKEY_TIME_ENSURE) {
                if (skey->getstate(skey->pdata)) {
                    // 消抖后确认按键按下
                    skey->flag |= SKEY_FLAG_PRESS_IN | SKEY_FLAG_PRESS;
                    skey->state = SKEY_STATE_PRESS;
                } else {
                    skey->state = SKEY_STATE_RELEASE;
                }
            }
            break;
        case SKEY_STATE_PRESS:
            if (!skey->getstate(skey->pdata)) {
                skey->timer = 0;
                skey->state = SKEY_STATE_RELEASE_PRE;
            } else {
                skey->timer += SKEY_TIME_UPDATE;
                if (skey->timer >= SKEY_TIME_HOLD) {
                    if (!(skey->flag & SKEY_FLAG_HOLD_IN)) {
                        // 如果未在长按键状态，则置位长按键标识
                        skey->flag |= SKEY_FLAG_HOLD_IN | SKEY_FLAG_HOLD;
                    } else if (skey->timer >= SKEY_TIME_HOLD + SKEY_TIME_REPEAT) {
                        // 已在长按键状态时超时置位重复标识
                        skey->timer -= SKEY_TIME_REPEAT;
                        skey->flag |= SKEY_FLAG_REPEAT_IN | SKEY_FLAG_REPEAT;
                    }
                }
            }
            break;
        case SKEY_STATE_RELEASE_PRE:
            skey->timer += SKEY_TIME_UPDATE;
            if (skey->timer >= SKEY_TIME_ENSURE) {
                if (!skey->getstate(skey->pdata)) {
                    skey->state = SKEY_STATE_RELEASE;
                    skey->flag &= ~SKEY_FLAG_IN_MASK;
                } else {
                    skey->state = SKEY_STATE_PRESS;
                }
            }
            break;
        default:
            break;
        }

#if SKEY_USE_SEM
        skey->sem = SKEY_SEM_FREE;
    }
#endif //SKEY_USE_SEM
}

// 更新按键数组
void SKey_UpdateArray(skey_t *skeys, uint8_t cnt)
{
    uint8_t i;

    for (i = 0; i < cnt; i++) {
        SKey_Update(skeys + i);
    }
}

/*
说明：获取按键标识状态
参数：flag: 要获取的标志，可多个位与, 详见key_flag_t定义
返回值：是否所指定的标志都置位
*/
BOOL SKey_CheckFlag(skey_t *skey, skey_flag_t flag)
{
    BOOL res;

    if (skey == NULL) {
        return FALSE;
    }

#if SKEY_USE_SEM
    if (SKEY_SEM_FREE == skey->sem) {
        skey->sem = SKEY_SEM_USING;
#endif //SKEY_USE_SEM

        res = ((skey->flag & flag) == flag);

        // 非内部标识读取后自动清除
        skey->flag &= ~(flag & ~SKEY_FLAG_IN_MASK);

#if SKEY_USE_SEM
        skey->sem = SKEY_SEM_FREE;
    } else {
        res = FLASE;
    }
#endif //SKEY_USE_SEM

    return res;
}
