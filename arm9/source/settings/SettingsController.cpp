#include "common.h"
#include "App.h"
#include "SettingsController.h"

SettingsController::SettingsController(IAppSettingsService* appSettingsService,
    TaskQueueBase* ioTaskQueue, BackCommittedSignal* backCommittedSignal)
    : _appSettingsService(appSettingsService), _ioTaskQueue(ioTaskQueue)
    , _backCommittedSignal(backCommittedSignal) { }

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
    _backCommittedSignal->CommitSettings(SettingsTransition::SelectTheme);
    _appSettingsService->GetAppSettings().theme = themeFolderName;
    _ioTaskQueue->Enqueue([this] (const vu8& cancelRequested)
    {
        _appSettingsService->Save();
        return TaskResult<void>::Completed();
    });
    gProcessManager.Goto<App>();
}
