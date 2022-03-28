#pragma once

#include <Common.h>

typedef struct {
    u8 _000[0x020 - 0x000];
    f32 internalSpeed;
    u8 _024[0x02c - 0x024];
    f32 hardSpeedLimit;
    u8 _030[0x074 - 0x030];
    Vec3 internalVelDir;
    u8 _080[0x1a8 - 0x080];
    s16 blinkTimer;
    u8 _1aa[0x294 - 0x1aa];
} KartMove;
static_assert(sizeof(KartMove) == 0x294);
