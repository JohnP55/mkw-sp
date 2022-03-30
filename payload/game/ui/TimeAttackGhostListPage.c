#include "TimeAttackGhostListPage.h"

#include "RaceConfirmPage.h"
#include "SectionManager.h"

#include "../system/RaceConfig.h"

#include <string.h>

static const Page_vt s_TimeAttackGhostListPage_vt;

static void onBack(InputHandler *this, u32 UNUSED(localPlayerId)) {
    TimeAttackGhostListPage *page = CONTAINER_OF(this, TimeAttackGhostListPage, onBack);
    page->replacement = PAGE_ID_COURSE_DETAIL;
    Page_startReplace(page, PAGE_ANIMATION_PREV, 0.0f);
}

static const InputHandler_vt onBack_vt = {
    .handle = onBack,
};

static void TimeAttackGhostListPage_refreshLaunchButton(TimeAttackGhostListPage *this) {
    if (this->chosenCount == 0) {
        this->switchLabel.isHidden = true;
        LayoutUIControl_setMessageAll(&this->launchButton, 6016, NULL);
    } else if (this->chosenCount == 1) {
        this->switchLabel.isHidden = false;
        u32 messageId = this->isReplay ? 10001 : 10000;
        LayoutUIControl_setMessageAll(&this->launchButton, messageId, NULL);
    } else {
        u32 messageId = this->isReplay ? 10003 : 10002;
        MessageInfo info = { .intVals[0] = this->chosenCount };
        LayoutUIControl_setMessageAll(&this->launchButton, messageId, &info);
    }
}

static void onOption(InputHandler *this, u32 UNUSED(localPlayerId)) {
    TimeAttackGhostListPage *page = CONTAINER_OF(this, TimeAttackGhostListPage, onOption);
    if (!page->switchLabel.isHidden) {
        page->isReplay = !page->isReplay;
        TimeAttackGhostListPage_refreshLaunchButton(page);
        Page_playSfx(page, 0x14 + !!page->isReplay, -1);
    }
}

static const InputHandler_vt onOption_vt = {
    .handle = onOption,
};

static bool TimeAttackGhostListPage_canSwapGhostSelects(TimeAttackGhostListPage *this) {
    if (!GhostSelectControl_isShown(this->shownGhostSelect)) {
        return false;
    }

    if (!GhostSelectControl_isHidden(this->hiddenGhostSelect)) {
        return false;
    }

    return true;
}

static void TimeAttackGhostListPage_refreshSheetLabel(TimeAttackGhostListPage *this) {
    this->sheetLabel.isHidden = this->sheetCount == 0;
    MessageInfo info = {
        .intVals[0] = this->sheetIndex + 1,
        .intVals[1] = this->sheetCount,
    };
    LayoutUIControl_setMessageAll(&this->sheetLabel, 2009, &info);
}

static void TimeAttackGhostListPage_swapGhostSelects(TimeAttackGhostListPage *this) {
    GhostSelectControl *tmp = this->shownGhostSelect;
    this->shownGhostSelect = this->hiddenGhostSelect;
    this->hiddenGhostSelect = tmp;
}

static void onSheetSelectRight(SheetSelectControlHandler *this, SheetSelectControl *UNUSED(control),
        u32 UNUSED(localPlayerId)) {
    TimeAttackGhostListPage *page = CONTAINER_OF(this, TimeAttackGhostListPage, onSheetSelectRight);
    if (!TimeAttackGhostListPage_canSwapGhostSelects(page)) {
        return;
    }

    if (page->sheetIndex == page->sheetCount - 1) {
        page->sheetIndex = 0;
    } else {
        page->sheetIndex++;
    }
    TimeAttackGhostListPage_refreshSheetLabel(page);

    GhostSelectControl_slideOutToLeft(page->shownGhostSelect);
    GhostSelectControl_slideInFromRight(page->hiddenGhostSelect);

    TimeAttackGhostListPage_swapGhostSelects(page);
}

static const SheetSelectControlHandler_vt onSheetSelectRight_vt = {
    .handle = onSheetSelectRight,
};

static void onSheetSelectLeft(SheetSelectControlHandler *this, SheetSelectControl *UNUSED(control),
        u32 UNUSED(localPlayerId)) {
    TimeAttackGhostListPage *page = CONTAINER_OF(this, TimeAttackGhostListPage, onSheetSelectLeft);
    if (!TimeAttackGhostListPage_canSwapGhostSelects(page)) {
        return;
    }

    if (page->sheetIndex == 0) {
        page->sheetIndex = page->sheetCount - 1;
    } else {
        page->sheetIndex--;
    }
    TimeAttackGhostListPage_refreshSheetLabel(page);

    GhostSelectControl_slideOutToRight(page->shownGhostSelect);
    GhostSelectControl_slideInFromLeft(page->hiddenGhostSelect);

    TimeAttackGhostListPage_swapGhostSelects(page);
}

static const SheetSelectControlHandler_vt onSheetSelectLeft_vt = {
    .handle = onSheetSelectLeft,
};

static void onLaunchButtonFront(PushButtonHandler *this, PushButton *UNUSED(button),
        u32 UNUSED(localPlayerId)) {
    TimeAttackGhostListPage *page =
            CONTAINER_OF(this, TimeAttackGhostListPage, onLaunchButtonFront);
    page->vt->push(page, PAGE_ID_RACE_CONFIRM, PAGE_ANIMATION_NEXT);
}

static const PushButtonHandler_vt onLaunchButtonFront_vt = {
    .handle = onLaunchButtonFront,
};

static void onLaunchButtonSelect(PushButtonHandler *this, PushButton *UNUSED(button),
        u32 UNUSED(localPlayerId)) {
    TimeAttackGhostListPage *page =
            CONTAINER_OF(this, TimeAttackGhostListPage, onLaunchButtonSelect);
    page->lastSelected = -1;
}

static const PushButtonHandler_vt onLaunchButtonSelect_vt = {
    .handle = onLaunchButtonSelect,
};

static void onBackButtonFront(PushButtonHandler *this, PushButton *button,
        u32 UNUSED(localPlayerId)) {
    TimeAttackGhostListPage *page =
            CONTAINER_OF(this, TimeAttackGhostListPage, onBackButtonFront);
    page->replacement = PAGE_ID_COURSE_DETAIL;
    f32 delay = PushButton_getDelay(button);
    Page_startReplace(page, PAGE_ANIMATION_PREV, delay);
}

static const PushButtonHandler_vt onBackButtonFront_vt = {
    .handle = onBackButtonFront,
};

static TimeAttackGhostListPage *my_TimeAttackGhostListPage_ct(TimeAttackGhostListPage *this) {
    Page_ct(this);
    this->vt = &s_TimeAttackGhostListPage_vt;

    MultiControlInputManager_ct(&this->inputManager);
    CtrlMenuPageTitleText_ct(&this->pageTitleText);
    LayoutUIControl_ct(&this->switchLabel);
    GhostSelectControl_ct(&this->ghostSelects[0]);
    GhostSelectControl_ct(&this->ghostSelects[1]);
    SheetSelectControl_ct(&this->sheetSelect);
    LayoutUIControl_ct(&this->sheetLabel);
    MessageWindowControl_ct(&this->messageWindow);
    PushButton_ct(&this->launchButton);
    CtrlMenuBackButton_ct(&this->backButton);
    this->onBack.vt = &onBack_vt;
    this->onOption.vt = &onOption_vt;
    this->onSheetSelectRight.vt = &onSheetSelectRight_vt;
    this->onSheetSelectLeft.vt = &onSheetSelectLeft_vt;
    this->onLaunchButtonSelect.vt = &onLaunchButtonSelect_vt;
    this->onLaunchButtonFront.vt = &onLaunchButtonFront_vt;
    this->onBackButtonFront.vt = &onBackButtonFront_vt;

    return this;
}
PATCH_B(TimeAttackGhostListPage_ct, my_TimeAttackGhostListPage_ct);

static void TimeAttackGhostListPage_dt(Page *base, s32 type) {
    TimeAttackGhostListPage *this = (TimeAttackGhostListPage *)base;

    CtrlMenuBackButton_dt(&this->backButton, -1);
    PushButton_dt(&this->launchButton, -1);
    MessageWindowControl_dt(&this->messageWindow, -1);
    LayoutUIControl_dt(&this->sheetLabel, -1);
    SheetSelectControl_dt(&this->sheetSelect, -1);
    GhostSelectControl_dt(&this->ghostSelects[1], -1);
    GhostSelectControl_dt(&this->ghostSelects[0], -1);
    LayoutUIControl_dt(&this->switchLabel, -1);
    CtrlMenuPageTitleText_dt(&this->pageTitleText, -1);
    MultiControlInputManager_dt(&this->inputManager, -1);

    Page_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

static s32 TimeAttackGhostListPage_getReplacement(Page *base) {
    TimeAttackGhostListPage *this = (TimeAttackGhostListPage *)base;

    return this->replacement;
}

static void TimeAttackGhostListPage_onInit(Page *base) {
    TimeAttackGhostListPage *this = (TimeAttackGhostListPage *)base;

    RaceConfigScenario *menuScenario = &s_raceConfig->menuScenario;
    const GlobalContext *cx = s_sectionManager->globalContext;
    switch (s_sectionManager->currentSection->id) {
    case SECTION_ID_SINGLE_CHANGE_COURSE:
    case SECTION_ID_SINGLE_CHANGE_GHOST_DATA:
        menuScenario->players[0].type = PLAYER_TYPE_LOCAL;
        menuScenario->players[0].vehicleId = cx->timeAttackVehicleId;
        menuScenario->players[0].characterId = cx->timeAttackCharacterId;
        break;
    }

    MultiControlInputManager_init(&this->inputManager, 0x1, false);
    this->baseInputManager = &this->inputManager;
    MultiControlInputManager_setWrappingMode(&this->inputManager, WRAPPING_MODE_Y);

    Page_initChildren(this, 9);
    Page_insertChild(this, 0, &this->pageTitleText, 0);
    Page_insertChild(this, 1, &this->switchLabel, 0);
    Page_insertChild(this, 2, &this->ghostSelects[0], 0);
    Page_insertChild(this, 3, &this->ghostSelects[1], 0);
    Page_insertChild(this, 4, &this->sheetSelect, 0);
    Page_insertChild(this, 5, &this->sheetLabel, 0);
    Page_insertChild(this, 6, &this->messageWindow, 0);
    Page_insertChild(this, 7, &this->launchButton, 0);
    Page_insertChild(this, 8, &this->backButton, 0);

    CtrlMenuPageTitleText_load(&this->pageTitleText, false);
    const char *groups[] = {
        NULL,
        NULL,
    };
    LayoutUIControl_load(&this->switchLabel, "control", "ClassChange", "ClassChange", groups);
    GhostSelectControl_load(&this->ghostSelects[0]);
    GhostSelectControl_load(&this->ghostSelects[1]);
    SheetSelectControl_load(&this->sheetSelect, "button", "TimeAttackGhostListArrowRight",
            "ButtonArrowRight", "TimeAttackGhostListArrowLeft", "ButtonArrowLeft", 0x1, false,
            false);
    LayoutUIControl_load(&this->sheetLabel, "control", "TimeAttackGhostListPageNum",
            "TimeAttackGhostListPageNum", NULL);
    MessageWindowControl_load(&this->messageWindow, "message_window", "MessageWindowHalf",
            "MessageWindowHalf");
    PushButton_load(&this->launchButton, "button", "TimeAttackGhostList", "Launch", 0x1, false,
            false);
    PushButton_load(&this->backButton, "button", "Back", "ButtonBackPopup", 0x1, false, true);

    MultiControlInputManager_setHandler(&this->inputManager, INPUT_ID_BACK, &this->onBack, false,
            false);
    MultiControlInputManager_setHandler(&this->inputManager, INPUT_ID_OPTION, &this->onOption,
            false, false);
    SheetSelectControl_setRightHandler(&this->sheetSelect, &this->onSheetSelectRight);
    SheetSelectControl_setLeftHandler(&this->sheetSelect, &this->onSheetSelectLeft);
    PushButton_setSelectHandler(&this->launchButton, &this->onLaunchButtonSelect);
    PushButton_setFrontHandler(&this->launchButton, &this->onLaunchButtonFront, false);
    PushButton_setFrontHandler(&this->backButton, &this->onBackButtonFront, false);

    CtrlMenuPageTitleText_setMessage(&this->pageTitleText, 3407, NULL);
    u32 flags = RegisteredPadManager_getFlags(&s_sectionManager->registeredPadManager, 0);
    u32 padType = REGISTERED_PAD_FLAGS_GET_TYPE(flags);
    u32 messageId = padType == REGISTERED_PAD_TYPE_GC ? 2306 : 2305;
    MessageInfo info = { .messageIds[0] = messageId };
    LayoutUIControl_setMessageAll(&this->switchLabel, 3012, &info);
    LayoutUIControl_setMessageAll(&this->messageWindow, 3101, NULL);
}

static void TimeAttackGhostListPage_onActivate(Page *base) {
    TimeAttackGhostListPage *this = (TimeAttackGhostListPage *)base;

    PushButton_selectDefault(&this->launchButton, 0);

    Section *currentSection = s_sectionManager->currentSection;
    this->ghostList = &((GhostManagerPage *)currentSection->pages[PAGE_ID_GHOST_MANAGER])->list;

    u32 buttonsPerSheet = ARRAY_SIZE(this->ghostSelects[0].buttons);
    this->sheetCount = (this->ghostList->count + buttonsPerSheet - 1) / buttonsPerSheet;
    this->sheetIndex = 0;
    TimeAttackGhostListPage_refreshSheetLabel(this);

    this->chosenCount = 0;
    for (u32 i = 0; i < ARRAY_SIZE(this->ghostIsChosen); i++) {
        this->ghostIsChosen[i] = false;
    }

    this->shownGhostSelect = &this->ghostSelects[0];
    this->hiddenGhostSelect = &this->ghostSelects[1];
    GhostSelectControl_hide(this->hiddenGhostSelect);
    GhostSelectControl_show(this->shownGhostSelect);

    this->lastSelected = -1;

    this->sheetSelect.isHidden = this->sheetCount <= 1;
    SheetSelectControl_setPlayerFlags(&this->sheetSelect, this->sheetCount <= 1 ? 0x0 : 0x1);

    this->isReplay = false;
    this->switchLabel.isHidden = true;
    TimeAttackGhostListPage_refreshLaunchButton(this);

    this->messageWindow.isHidden = this->sheetCount != 0;

    this->replacement = PAGE_ID_NONE;
}

static void TimeAttackGhostListPage_onRefocus(Page *base) {
    TimeAttackGhostListPage *this = (TimeAttackGhostListPage *)base;

    Section *currentSection = s_sectionManager->currentSection;
    if (!((RaceConfirmPage *)currentSection->pages[PAGE_ID_RACE_CONFIRM])->hasConfirmed) {
        return;
    }

    RaceConfigScenario *menuScenario = &s_raceConfig->menuScenario;
    u32 playerCount = this->chosenCount;
    if (playerCount == 0 || !this->isReplay) {
        playerCount++;
    }
    for (u32 i = playerCount; i < ARRAY_SIZE(menuScenario->players); i++) {
        menuScenario->players[i].type = PLAYER_TYPE_NONE;
    }

    GlobalContext *cx = s_sectionManager->globalContext;
    cx->timeAttackGhostType = GHOST_TYPE_SAVED;
    cx->timeAttackCourseId = menuScenario->courseId;
    cx->timeAttackLicenseId = -1;

    cx->timeAttackGhostCount = 0;
    for (u32 i = 0; i < ARRAY_SIZE(this->ghostIsChosen); i++) {
        if (this->ghostIsChosen[i]) {
            u32 ghostIndex = this->ghostList->indices[i];
            cx->timeAttackGhostIndices[cx->timeAttackGhostCount++] = ghostIndex;
        }
    }

    GhostManagerPage *ghostManagerPage =
            (GhostManagerPage *)currentSection->pages[PAGE_ID_GHOST_MANAGER];
    u32 sectionId;
    if (this->chosenCount == 0) {
        sectionId = SECTION_ID_TIME_ATTACK;
    } else if (this->isReplay) {
        ghostManagerPage->nextRequest = GHOST_MANAGER_PAGE_REQUEST_SAVED_GHOST_REPLAY;
        sectionId = SECTION_ID_GHOST_REPLAY;
    } else {
        ghostManagerPage->nextRequest = GHOST_MANAGER_PAGE_REQUEST_SAVED_GHOST_RACE;
        sectionId = SECTION_ID_TIME_ATTACK;
    }
    this->vt->changeSection(this, sectionId, PAGE_ANIMATION_NEXT, 0.0f);
}

static const Page_vt s_TimeAttackGhostListPage_vt = {
    .dt = TimeAttackGhostListPage_dt,
    .vf_0c = &Page_vf_0c,
    .getReplacement = TimeAttackGhostListPage_getReplacement,
    .vf_14 = &Page_vf_14,
    .vf_18 = &Page_vf_18,
    .changeSection = Page_changeSection,
    .vf_20 = &Page_vf_20,
    .push = Page_push,
    .onInit = TimeAttackGhostListPage_onInit,
    .onDeinit = Page_onDeinit,
    .onActivate = TimeAttackGhostListPage_onActivate,
    .vf_34 = &Page_vf_34,
    .vf_38 = &Page_vf_38,
    .vf_3c = &Page_vf_3c,
    .vf_40 = &Page_vf_40,
    .vf_44 = &Page_vf_44,
    .beforeCalc = Page_beforeCalc,
    .afterCalc = Page_afterCalc,
    .vf_50 = &Page_vf_50,
    .onRefocus = TimeAttackGhostListPage_onRefocus,
    .vf_58 = &Page_vf_58,
    .vf_5c = &Page_vf_5c,
    .getTypeInfo = Page_getTypeInfo,
};

void TimeAttackGhostListPage_chooseGhost(TimeAttackGhostListPage *this, u32 buttonIndex) {
    u32 listIndex = this->sheetIndex * ARRAY_SIZE(this->ghostSelects[0].buttons) + buttonIndex;
    this->ghostIsChosen[listIndex] = this->shownGhostSelect->buttons[buttonIndex].chosen;

    if (this->shownGhostSelect->buttons[buttonIndex].chosen) {
        this->chosenCount++;
    } else {
        this->chosenCount--;
    }

    TimeAttackGhostListPage_refreshLaunchButton(this);
}
