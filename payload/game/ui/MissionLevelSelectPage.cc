#include "game/ui/MissionLevelSelectPage.hh"

namespace UI {

MissionLevelSelectPage::MissionLevelSelectPage() = default;
MissionLevelSelectPage::~MissionLevelSelectPage() = default;

PageId MissionLevelSelectPage::getReplacement() {
    return m_replacement;
}

void MissionLevelSelectPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);

    m_pageTitleText.load(false);
    m_pageTitleText.setMessage(10160);
}

void MissionLevelSelectPage::onActivate() {
    SP_LOG("heyyyyyyyy");
}

void MissionLevelSelectPage::onBack(u32 localPlayerId) {
    m_replacement = m_prevId;
    Page::startReplace(Anim::Prev, 0.0f);
}
}