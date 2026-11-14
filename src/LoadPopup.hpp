#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <filesystem>
#include <vector>

using namespace geode::prelude;

namespace macrobot {

// Lists every .mbf file in the mod's macros folder. Each row has:
//   [ name ]  -> loads the macro and immediately starts playing it
//   [ Edit ]  -> rename
//   [ Del  ]  -> delete (with confirmation)
// The bottom buttons open the macros folder (drop .mbf files in to import,
// copy them out to export) and refresh the list.
//
// See BotMenu.hpp for the note on the current (non-template) geode::Popup API.
class LoadPopup : public geode::Popup {
protected:
    bool init() override;
    void onPick(CCObject* sender);
    void onRename(CCObject* sender);
    void onDelete(CCObject* sender);
    void onOpenFolder(CCObject*);
    void onRefresh(CCObject*);

    std::vector<std::filesystem::path> m_files;

public:
    static LoadPopup* create();
};

}
