#include "LoadPopup.hpp"
#include "BotMenu.hpp"
#include "RenamePopup.hpp"
#include "MacroManager.hpp"

using namespace geode::prelude;

namespace macrobot {

LoadPopup* LoadPopup::create() {
    auto ret = new LoadPopup();

    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

static std::string shortName(const std::string& name) {
    constexpr size_t kMax = 15;
    if (name.size() <= kMax) return name;
    return name.substr(0, kMax) + "..";
}

bool LoadPopup::init() {
    if (!Popup::init(320.f, 240.f)) return false;

    this->setTitle("Macros");

    auto size = m_mainLayer->getContentSize();
    auto& mgr = MacroManager::get();

    m_files = mgr.listMacros();

    // --- Bottom buttons (always shown, even with an empty list, so the
    // folder can be opened to import files) -------------------------------
    auto bottomMenu = CCMenu::create();
    bottomMenu->setPosition({0.f, 0.f});
    m_mainLayer->addChild(bottomMenu);

    auto folderSprite = ButtonSprite::create("Open Folder", "bigFont.fnt", "GJ_button_02.png", 0.6f);
    auto folderBtn = CCMenuItemSpriteExtra::create(
        folderSprite, this, menu_selector(LoadPopup::onOpenFolder)
    );
    folderBtn->setPosition({size.width * 0.5f - 60.f, 22.f});
    bottomMenu->addChild(folderBtn);

    auto refreshSprite = ButtonSprite::create("Refresh", "bigFont.fnt", "GJ_button_01.png", 0.6f);
    auto refreshBtn = CCMenuItemSpriteExtra::create(
        refreshSprite, this, menu_selector(LoadPopup::onRefresh)
    );
    refreshBtn->setPosition({size.width * 0.5f + 60.f, 22.f});
    bottomMenu->addChild(refreshBtn);

    if (m_files.empty()) {
        auto label = CCLabelBMFont::create(
            "No saved macros yet",
            "chatFont.fnt"
        );

        label->setScale(0.6f);
        label->setPosition({
            size.width * 0.5f,
            size.height * 0.5f
        });

        m_mainLayer->addChild(label);
        return true;
    }

    // --- Scrolling list: one row per macro -----------------------------
    auto listSize = CCSize{
        size.width - 30.f,
        size.height - 100.f
    };

    auto scroll = ScrollLayer::create(listSize);

    scroll->setPosition({
        (size.width - listSize.width) / 2.f,
        45.f
    });

    m_mainLayer->addChild(scroll);

    float rowHeight = 30.f;
    float rowGap = 6.f;
    float rowStep = rowHeight + rowGap;
    float y = static_cast<float>(m_files.size()) * rowStep;

    for (size_t i = 0; i < m_files.size(); i++) {
        auto name = shortName(m_files[i].stem().string());

        auto nameSprite = ButtonSprite::create(
            name.c_str(), 170, true, "bigFont.fnt", "GJ_button_01.png", 25.f, 0.5f
        );
        auto nameBtn = CCMenuItemSpriteExtra::create(
            nameSprite, this, menu_selector(LoadPopup::onPick)
        );
        nameBtn->setTag(static_cast<int>(i));

        auto editSprite = ButtonSprite::create(
            "Edit", 50, true, "bigFont.fnt", "GJ_button_02.png", 25.f, 0.5f
        );
        auto editBtn = CCMenuItemSpriteExtra::create(
            editSprite, this, menu_selector(LoadPopup::onRename)
        );
        editBtn->setTag(static_cast<int>(i));

        auto delSprite = ButtonSprite::create(
            "Del", 40, true, "bigFont.fnt", "GJ_button_06.png", 25.f, 0.5f
        );
        auto delBtn = CCMenuItemSpriteExtra::create(
            delSprite, this, menu_selector(LoadPopup::onDelete)
        );
        delBtn->setTag(static_cast<int>(i));

        auto rowMenu = CCMenu::create();
        rowMenu->setContentSize({
            listSize.width,
            rowHeight
        });

        rowMenu->addChild(nameBtn);
        rowMenu->addChild(editBtn);
        rowMenu->addChild(delBtn);

        nameBtn->setPosition({95.f, rowHeight * 0.5f});
        editBtn->setPosition({212.f, rowHeight * 0.5f});
        delBtn->setPosition({262.f, rowHeight * 0.5f});

        rowMenu->setPosition({
            0.f,
            y - rowStep
        });

        scroll->m_contentLayer->addChild(rowMenu);

        y -= rowStep;
    }

    scroll->m_contentLayer->setContentSize({
        listSize.width,
        static_cast<float>(m_files.size()) * rowStep
    });

    scroll->moveToTop();

    return true;
}

// Converts a macro filename / level name into the same sanitized form
// MacroManager uses when creating filenames.
//
// A final " (number)" suffix is ignored. This means:
//
//   Stereo Madness
//   Stereo Madness (2)
//   Stereo Madness (3)
//
// are all considered to belong to the same level for the compatibility
// warning.
static std::string normalizedLevelFileName(std::string name) {
    if (name.size() > 4 && name.back() == ')') {
        auto open = name.rfind(" (");

        if (open != std::string::npos && open + 3 < name.size()) {
            bool allDigits = true;

            for (size_t i = open + 2; i + 1 < name.size(); ++i) {
                if (!std::isdigit(
                        static_cast<unsigned char>(name[i])
                    )) {
                    allDigits = false;
                    break;
                }
            }

            if (allDigits) {
                name.erase(open);
            }
        }
    }

    // MacroManager sanitizes level names before using them as filenames.
    std::string sanitized;
    sanitized.reserve(name.size());

    for (char c : name) {
        if (
            std::isalnum(static_cast<unsigned char>(c)) ||
            c == ' ' ||
            c == '-' ||
            c == '_'
        ) {
            sanitized.push_back(c);
        } else {
            sanitized.push_back('_');
        }
    }

    return sanitized.empty() ? "macro" : sanitized;
}

void LoadPopup::onPick(CCObject* sender) {
    auto item = static_cast<CCMenuItemSpriteExtra*>(sender);
    size_t idx = static_cast<size_t>(item->getTag());

    if (idx >= m_files.size()) return;

    auto& mgr = MacroManager::get();
    auto path = m_files[idx];

    // Get the current level's actual name.
    std::string currentLevel;

    if (auto pl = PlayLayer::get()) {
        if (pl->m_level) {
            currentLevel = pl->m_level->m_levelName;
        }
    }

    // Compare the macro filename against the current level.
    //
    // The saved filename is used rather than the internal MacroData
    // levelName because the user's requested behavior is specifically
    // based on names such as:
    //
    //     Level
    //     Level (2)
    //     Level (3)
    //
    // where the numeric suffix only means another macro was saved for
    // that same level.
    auto macroName = path.stem().string();

    bool levelMatches =
        !currentLevel.empty() &&
        normalizedLevelFileName(macroName) ==
            normalizedLevelFileName(currentLevel);

    // Loading the macro itself is kept in one callback so that both the
    // normal path and the confirmation path behave identically after
    // the user has made their choice.
    auto loadAndPlay = [this, path]() {
        auto& mgr = MacroManager::get();

        if (!mgr.loadMacroFromFile(path)) {
            FLAlertLayer::create(
                "MacroPath",
                "Failed to load that macro file.",
                "OK"
            )->show();

            return;
        }

        // Picking a macro loads it and immediately starts playback.
        beginPlayback();

        this->keyBackClicked();
        BotMenuPopup::closeIfOpen();
    };

    // If we don't know the current level name, don't block loading.
    //
    // Otherwise, a mismatch requires explicit confirmation before the
    // loaded macro replaces the currently armed macro.
    if (!levelMatches && !currentLevel.empty()) {
        geode::createQuickPopup(
            "MacroPath",
            "Are you sure you want to replace the current recorded inputs?",
            "Cancel",
            "Yes",
            [loadAndPlay](auto, bool confirmed) {
                if (confirmed) {
                    loadAndPlay();
                }
            }
        );

        return;
    }

    loadAndPlay();
}

void LoadPopup::onRename(CCObject* sender) {
    auto item = static_cast<CCMenuItemSpriteExtra*>(sender);
    size_t idx = static_cast<size_t>(item->getTag());
    if (idx >= m_files.size()) return;

    RenamePopup::create(m_files[idx], [this]() {
        // Reopen the list so it shows the new name.
        this->keyBackClicked();
        LoadPopup::create()->show();
    })->show();
}

void LoadPopup::onDelete(CCObject* sender) {
    auto item = static_cast<CCMenuItemSpriteExtra*>(sender);
    size_t idx = static_cast<size_t>(item->getTag());
    if (idx >= m_files.size()) return;

    auto path = m_files[idx];

    geode::createQuickPopup(
        "MacroPath",
        "Delete \"" + path.stem().string() + "\"? This can't be undone.",
        "Cancel",
        "Delete",
        [this, path](auto, bool confirmed) {
            if (!confirmed) return;

            if (!MacroManager::get().deleteMacro(path)) {
                FLAlertLayer::create(
                    "MacroPath",
                    "Failed to delete that macro file.",
                    "OK"
                )->show();
                return;
            }

            this->keyBackClicked();
            LoadPopup::create()->show();
        }
    );
}

void LoadPopup::onOpenFolder(CCObject*) {
    // Import: drop .mbf files into this folder, then hit Refresh.
    // Export: copy .mbf files out of it.
    (void)geode::utils::file::openFolder(MacroManager::get().macrosDir());
}

void LoadPopup::onRefresh(CCObject*) {
    this->keyBackClicked();
    LoadPopup::create()->show();
}

}
