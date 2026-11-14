#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace macrobot {

// Shared by BotMenuPopup's Play button and LoadPopup's pick handler:
// resets the current level and arms the manager to play back whatever
// macro is currently loaded/armed.
void beginPlayback();

// The bot menu panel that opens when the circular button in the pause
// menu is pressed. Four buttons (Record, Save, Play, Load) plus toggles
// for Auto-save, Block inputs while playing, and a Debug HUD that
// shows/hides the in-level frame + event counters.
//
// NOTE: this is a plain, self-contained CCLayer — NOT geode::Popup /
// FLAlertLayer. An earlier revision tried to reposition a Popup to a
// fixed bottom-left spot, which broke badly (split card/content,
// unclosable): Popup is internally a full-screen overlay whose card
// elements are positioned at window-center by code we don't control, so
// moving the outer node doesn't move everything cleanly and can desync
// its own hit-testing. Building this panel ourselves means the position,
// background, and close behavior are all things we directly control —
// no inherited surprises.
class BotMenuPopup : public CCLayer {
protected:
    bool init();
    void keyBackClicked() override;
    void refreshButtonStates();

    void onRecord(CCObject*);
    void onSave(CCObject*);
    void onPlay(CCObject*);
    void onLoad(CCObject*);
    void onToggleHud(CCObject*);
    void onToggleAutoSave(CCObject*);
    void onToggleBlockInputs(CCObject*);
    void onCloseClicked(CCObject*);

    void closeSelf();

    // Adds a checkbox + label row at height `y`; returns the toggler.
    CCMenuItemToggler* makeToggle(CCMenu* menu, const char* label, float y, SEL_MenuHandler handler);

    CCMenuItemSpriteExtra* m_recordBtn = nullptr;
    CCMenuItemSpriteExtra* m_saveBtn = nullptr;
    CCMenuItemSpriteExtra* m_playBtn = nullptr;
    CCMenuItemToggler* m_hudToggle = nullptr;
    CCMenuItemToggler* m_autoSaveToggle = nullptr;
    CCMenuItemToggler* m_blockInputsToggle = nullptr;
    CCLabelBMFont* m_statusLabel = nullptr;

    static BotMenuPopup* s_current;

public:
    static BotMenuPopup* create();
    // Adds this panel to the current scene, fixed at the bottom-left.
    void show();
    // Closes the currently-open bot menu, if any. Used by LoadPopup so
    // picking a macro visibly returns to the bot menu before it auto-
    // closes via Play (matches the spec: load -> back to bot menu ->
    // auto-click play).
    static void closeIfOpen();
};

}
