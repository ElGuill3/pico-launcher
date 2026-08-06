#include <cassert>

#include "BackCommittedSignal.h"

int main()
{
    BackCommittedSignal signal;

    signal.Commit(BackTransition::Accepted);
    assert(signal.Consume());
    assert(!signal.Consume());

    signal.Commit(BackTransition::Rejected);
    signal.Commit(BackTransition::Forward);
    assert(!signal.Consume());

    signal.CommitFolderBack(true, false, false);
    signal.CommitFolderBack(true, true, false);
    signal.CommitFolderBack(false, true, true);
    assert(!signal.Consume());
    signal.CommitFolderBack(true, true, true);
    assert(signal.Consume());
    assert(!signal.Consume());

    signal.CommitCheatsBack(true);
    assert(signal.Consume());
    assert(!signal.Consume());

    signal.Commit(BackTransition::Accepted);
    signal.CommitCheatsBack(false);
    assert(signal.Consume());
    assert(!signal.Consume());

    signal.CommitSettings(SettingsTransition::SelectTheme);
    assert(!signal.Consume());
    signal.CommitSettings(SettingsTransition::Back);
    assert(signal.Consume());
    assert(!signal.Consume());
}
