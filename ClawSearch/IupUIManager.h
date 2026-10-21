#pragma once
#include "IUIManager.h"
#include <iup.h>

class IupUIManager : public IUIManager {
public:
    void ShowMessage(const char* message) override { IupMessage("Message", message); }
    void SetButtonActive(const char* buttonName, bool active) override {
        Ihandle* h = IupGetHandle(buttonName);
        if (h) IupSetAttribute(h, "ACTIVE", active ? "YES" : "NO");
    }
    void SetInputValue(const char* inputName, const char* value) override {
        Ihandle* h = IupGetHandle(inputName);
        if (h) IupSetAttribute(h, "VALUE", value);
    }
    const char* GetInputValue(const char* inputName) override {
        Ihandle* h = IupGetHandle(inputName);
        return h ? IupGetAttribute(h, "VALUE") : "";
    }
    void ClearList(const char* listName) override {
        Ihandle* h = IupGetHandle(listName);
        if (h) IupSetAttribute(h, "REMOVEITEM", "ALL");
    }
    void AppendItemToList(const char* listName, const char* item) override {
        Ihandle* h = IupGetHandle(listName);
        if (h) IupSetAttribute(h, "APPENDITEM", item);
    }
    void RedrawList(const char* listName) override {
        Ihandle* h = IupGetHandle(listName);
        if (h) {
            IupSetAttribute(h, "AUTOREDRAW", "YES");
            IupRedraw(h, 1);
        }
    }
    void CreateButton(const char* name, const char* label, void (*callback)()) override {}
    void CreateToggle(const char* name, const char* label) override {}
    void CreateList(const char* name) override {}
    void CreateText(const char* name) override {}
    void Render() override {}
};
