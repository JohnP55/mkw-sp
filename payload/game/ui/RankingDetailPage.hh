#pragma once

#include <common.h>
#include "game/ui/Page.hh"
#include "game/util/Registry.hh"
#include "SPRankingTopTenDownloadPage.hh"

namespace UI {

class RankingDetailPage : public Page {
public:
    void initRaceLeaderboardGlobal(Registry::Course course, UI::RankingPage::Area scope, RankingTopTenDownloadPageTimesArray *timerArray);
private:
    u8 _44[0x1754-0x44];
};
static_assert(sizeof(RankingDetailPage) == 0x1754);

}