#include "SPRankingTopTenDownloadPage.hh"

#include "game/ui/AwaitPage.hh"
#include "game/ui/MessagePage.hh"
#include "game/ui/RankingDetailPage.hh"
#include "game/ui/SectionManager.hh"

#include <cstdio>

namespace UI {

SPRankingTopTenDownloadPage::SPRankingTopTenDownloadPage() = default;

SPRankingTopTenDownloadPage::~SPRankingTopTenDownloadPage() = default;

void SPRankingTopTenDownloadPage::onActivate() {
    auto *section = SectionManager::Instance()->currentSection();

    m_state = State::Previous;
    switch (section->page<PageId::Ranking>()->area()) {
    case RankingPage::Area::Friend:
        transition(State::InDevelopment);
        break;
    case RankingPage::Area::Regional:
    case RankingPage::Area::Worldwide:
        char url[90];
        snprintf(url, sizeof(url), "http://127.0.0.1:5299/api/GetTopTenRankings?region=%d&course=%d",
                getRegionParameterValue(), getCourseParameterValue());
        if (makeRequest(url)) {
            transition(State::Request);
        } else {
            transition(State::Response);
        }
        break;
    }
}

void SPRankingTopTenDownloadPage::transition(State state) {
    auto *section = SectionManager::Instance()->currentSection();
    auto *menuMessagePage = section->page<PageId::MenuMessage>();
    auto *spinnerAwaitPage = section->page<PageId::SpinnerAwait>();

    if (state == m_state) {
        return;
    }

    switch (state) {
    case State::Previous:
        m_replacement = PageId::Ranking;
        startReplace(Anim::Next, 0.0f);
        break;
    case State::InDevelopment:
        menuMessagePage->reset();
        menuMessagePage->setWindowMessage(10430);
        push(PageId::MenuMessage, Anim::Next);
        break;
    case State::Request:
        spinnerAwaitPage->reset();
        spinnerAwaitPage->setWindowMessage(6114);
        push(PageId::SpinnerAwait, Anim::Next);
        break;
    case State::Response:
        menuMessagePage->reset();
        switch (responseStatus()) {
        case ResponseStatus::Ok:
            sendToRankingDetailPage();
            push(PageId::RankingDetail, Anim::Next);
            break;
        case ResponseStatus::RequestError:
            menuMessagePage->setWindowMessage(10448);
            push(PageId::MenuMessage, Anim::Next);
            break;
        case ResponseStatus::ResponseError:
            menuMessagePage->setWindowMessage(10449);
            push(PageId::MenuMessage, Anim::Next);
            break;
        }
        break;
    case State::Finished:
        m_replacement = PageId::Ranking;
        startReplace(Anim::Prev, 0.0f);
        break;
    }

    m_state = state;
}

void SPRankingTopTenDownloadPage::sendToRankingDetailPage() {
    auto section = SectionManager::Instance()->currentSection();
    auto *rankingDetailPage = section->page<PageId::RankingDetail>();
    auto leaderboards = rankingResponse();
    timeArr.count = leaderboards.players_count;
    for (int i = 0; i < timeArr.count; i++) {
        auto timeEntry = &timeArr.times[i];
        auto responseEntry = &leaderboards.players[i];

        timeEntry->minutes = responseEntry->time / 60000;
        timeEntry->seconds = responseEntry->time / 1000 % 60;
        timeEntry->milliseconds = responseEntry->time % 1000;

        //timeEntry->controller = responseEntry->
        // add the mii lolxd
    }
    rankingDetailPage->initRaceLeaderboardGlobal(section->page<PageId::Ranking>()->course(), section->page<PageId::Ranking>()->area(), &timeArr);
}

} // namespace UI
