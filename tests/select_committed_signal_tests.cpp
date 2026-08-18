#include <cassert>

#include "SelectCommittedSignal.h"

int main()
{
    SelectCommittedSignal signal;

    signal.Commit(SelectTransition::Accepted);
    assert(signal.Consume());
    assert(!signal.Consume());

    signal.Commit(SelectTransition::Rejected);
    signal.Commit(SelectTransition::Backward);
    assert(!signal.Consume());

    signal.CommitFolderEntry(false, true, true, true, true);
    signal.CommitFolderEntry(true, false, true, true, true);
    signal.CommitFolderEntry(true, true, false, true, true);
    signal.CommitFolderEntry(true, true, true, false, true);
    signal.CommitFolderEntry(true, true, true, true, false);
    assert(!signal.Consume());
    signal.CommitFolderEntry(true, true, true, true, true);
    assert(signal.Consume());

    signal.CommitLaunchReady(false);
    signal.CommitCheatMutation(false);
    assert(!signal.Consume());
    signal.CommitLaunchReady(true);
    assert(signal.Consume());
    assert(!signal.Consume());
    signal.CommitCheatMutation(true);
    assert(signal.Consume());
    assert(!signal.Consume());

    signal.Commit(SelectTransition::Accepted);
    signal.CommitCheatMutation(true);
    assert(signal.Consume());
    assert(!signal.Consume());

    assert(ResolveUiSound(false, false, false) == UiSound::None);
    assert(ResolveUiSound(false, false, true) == UiSound::Navigation);
    assert(ResolveUiSound(false, true, true) == UiSound::Select);
    assert(ResolveUiSound(true, true, true) == UiSound::Back);
}
