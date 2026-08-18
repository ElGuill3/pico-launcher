#pragma once

enum class SelectTransition
{
    Rejected,
    Accepted,
    Backward
};

enum class UiSound
{
    None,
    Navigation,
    Select,
    Back
};

constexpr UiSound ResolveUiSound(bool backCommitted, bool selectCommitted,
    bool navigationCommitted)
{
    return backCommitted ? UiSound::Back :
        selectCommitted ? UiSound::Select :
        navigationCommitted ? UiSound::Navigation : UiSound::None;
}

class SelectCommittedSignal
{
public:
    void Commit(SelectTransition transition)
    {
        _committed |= transition == SelectTransition::Accepted;
    }

    void CommitFolderEntry(bool requestedForward, bool targetResolved, bool chdirSucceeded,
        bool directoryChanged, bool folderLoaded)
    {
        _committed |= requestedForward && targetResolved && chdirSucceeded &&
            directoryChanged && folderLoaded;
    }

    void CommitLaunchReady(bool ready)
    {
        _committed |= ready;
    }

    void CommitCheatMutation(bool changed)
    {
        _committed |= changed;
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
