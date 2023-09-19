#pragma once

#include <common.h>
#include "game/ui/SPRankingDownloadPage.hh"
#include "game/util/Registry.hh"

namespace UI {

struct RankingMii {
    u8 _00[0x4c];
};
class RankingTopTenTimer {
private:
    void* _vt;
public:
    s16 minutes;
    s8 seconds;
    u8 _padding_03;
    s16 milliseconds;
    u16 __padding_06;
    RankingMii mii;
    Registry::Controller controller;
    u8 _58[0x60-0x58];
    u8 country;
    u8 state;
    s16 location; // unused
};

static_assert(sizeof(RankingTopTenTimer) == 0x68);

struct RankingTopTenDownloadPageTimesArray {
    int count;
    RankingTopTenTimer times[10];
};

class SPRankingTopTenDownloadPage : public SPRankingDownloadPage {
public:
    SPRankingTopTenDownloadPage();
    ~SPRankingTopTenDownloadPage() override;

    void onActivate() override;

private:
    void transition(State state) override;
    void sendToRankingDetailPage();
    RankingTopTenDownloadPageTimesArray timeArr;
};

} // namespace UI
