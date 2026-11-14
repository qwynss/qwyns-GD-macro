#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <filesystem>
#include <functional>

using namespace geode::prelude;

namespace macrobot {

// Small popup with a text box for renaming a saved macro file.
// `onDone` runs after a successful rename (the popup has already closed).
class RenamePopup : public geode::Popup {
protected:
    bool init() override;
    void onConfirm(CCObject*);

    std::filesystem::path m_path;
    std::function<void()> m_onDone;
    TextInput* m_input = nullptr;

public:
    static RenamePopup* create(std::filesystem::path path, std::function<void()> onDone);
};

}
