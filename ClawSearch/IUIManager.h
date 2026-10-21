#pragma once

// Abstraktionsschicht für die GUI, um IUP von der Geschäftslogik zu trennen.
class IUIManager {
public:
    virtual ~IUIManager() = default;

    virtual void ShowMessage(const char* message) = 0;
    virtual void SetButtonActive(const char* buttonName, bool active) = 0;
    virtual void SetInputValue(const char* inputName, const char* value) = 0;
    virtual const char* GetInputValue(const char* inputName) = 0;
    
    virtual void ClearList(const char* listName) = 0;
    virtual void AppendItemToList(const char* listName, const char* item) = 0;
    virtual void RedrawList(const char* listName) = 0;

    virtual void CreateButton(const char* name, const char* label, void (*callback)()) = 0;
    virtual void CreateToggle(const char* name, const char* label) = 0;
    virtual void CreateList(const char* name) = 0;
    virtual void CreateText(const char* name) = 0;
    virtual void Render() = 0;
};
