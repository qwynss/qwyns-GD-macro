#include "RenamePopup.hpp"
#include "MacroManager.hpp"

using namespace geode::prelude;

namespace macrobot {

RenamePopup* RenamePopup::create(std::filesystem::path path, std::function<void()> onDone) {
    auto ret = new RenamePopup();
    ret->m_path = std::move(path);
    ret->m_onDone = std::move(onDone);

    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool RenamePopup::init() {
    if (!Popup::init(260.f, 140.f)) return false;

    this->setTitle("Rename Macro");

    auto size = m_mainLayer->getContentSize();

    m_input = TextInput::create(200.f, "Macro name", "bigFont.fnt");
    m_input->setMaxCharCount(40);
    m_input->setString(m_path.stem().string());
    m_input->setPosition({size.width * 0.5f, size.height * 0.55f});
    m_mainLayer->addChild(m_input);

    auto menu = CCMenu::create();
    menu->setPosition({0.f, 0.f});
    m_mainLayer->addChild(menu);

    auto confirmSprite = ButtonSprite::create("Rename", "bigFont.fnt", "GJ_button_01.png", 0.7f);
    auto confirmBtn = CCMenuItemSpriteExtra::create(
        confirmSprite, this, menu_selector(RenamePopup::onConfirm)
    );
    confirmBtn->setPosition({size.width * 0.5f, 26.f});
    menu->addChild(confirmBtn);

    return true;
}

void RenamePopup::onConfirm(CCObject*) {
    auto name = m_input->getString();

    if (!MacroManager::get().renameMacro(m_path, name)) {
        FLAlertLayer::create(
            "MacroPath",
            "Couldn't rename that macro. A macro with that name may already exist.",
            "OK"
        )->show();
        return;
    }

    auto done = m_onDone;
    this->keyBackClicked();
    if (done) done();
}

}
