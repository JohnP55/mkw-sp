#pragma once

#include "game/ui/page/MenuPage.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class MissionLevelSelectPage : public Page {
public:
    MissionLevelSelectPage();
    MissionLevelSelectPage(const MissionLevelSelectPage &) = delete;
    MissionLevelSelectPage(MissionLevelSelectPage &&) = delete;
    ~MissionLevelSelectPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

    PageId m_prevId;
private:
    PageId m_replacement;

    CtrlMenuPageTitleText m_pageTitleText;


    template <typename T>
    using H = typename T::Handler<MissionLevelSelectPage>;

    void onBack(u32 localPlayerId);

    MultiControlInputManager m_inputManager;
    H<MultiControlInputManager> m_onBack{this, &MissionLevelSelectPage::onBack};
};

}
