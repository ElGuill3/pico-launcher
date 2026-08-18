#include "common.h"
#include "App.h"
#include "SettingsController.h"

SettingsController::SettingsController(IAppSettingsService* appSettingsService,
    TaskQueueBase* ioTaskQueue, BackCommittedSignal* backCommittedSignal,
    SelectCommittedSignal* selectCommittedSignal)
    : _appSettingsService(appSettingsService), _ioTaskQueue(ioTaskQueue)
    , _backCommittedSignal(backCommittedSignal)
    , _selectCommittedSignal(selectCommittedSignal) { }

void SettingsController::Initialize()
{
    _themeRepository.Initialize();
    _themeInfoManager = std::make_unique<ThemeInfoManager>(_themeRepository);
}

void SettingsController::NavigateUp()
{
    _backCommittedSignal->CommitSettings(SettingsTransition::Back);
    gProcessManager.Goto<App>();
}

void SettingsController::SelectTheme(const char* themeFolderName)
{
    _selectCommittedSignal->Commit(SelectTransition::Accepted);
    _appSettingsService->GetAppSettings().theme = themeFolderName;
    _ioTaskQueue->Enqueue([this] (const vu8& cancelRequested)
    {
        _appSettingsService->Save();
        return TaskResult<void>::Completed();
    });
    gProcessManager.Goto<App>();
}
