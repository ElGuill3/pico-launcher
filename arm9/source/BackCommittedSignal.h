#pragma once

enum class BackTransition
{
    Rejected,
    Accepted,
    Forward
};

enum class SettingsTransition
{
    Back,
    SelectTheme
};

class BackCommittedSignal
{
public:
    void Commit(BackTransition transition)
    {
        _committed |= transition == BackTransition::Accepted;
    }

    void CommitFolderBack(bool requestedBack, bool chdirSucceeded, bool directoryChanged)
    {
        _committed |= requestedBack && chdirSucceeded && directoryChanged;
    }

    void CommitCheatsBack(bool poppedSubcategory)
    {
        _committed |= poppedSubcategory;
    }

    void CommitSettings(SettingsTransition transition)
    {
        _committed |= transition == SettingsTransition::Back;
    }

    bool Consume()
    {
        const bool committed = _committed;
        _committed = false;
        return committed;
    }

private:
    bool _committed = false;
};
