//main.cpp
#include <rgde/engine.h>
#include "TestCamera.h"

int __stdcall WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    std::auto_ptr<core::IApplication> pApp(core::IApplication::Create(L"Cameras", 800, 600, false, false));
    pApp->addTask(core::PTask(new core::InputTask(*pApp, 0, false)));
    pApp->addTask(core::PTask(new core::CGameTask(*pApp, 1)));
    pApp->addTask(core::PTask(new core::RenderTask(*pApp, 2)));

    TestCamera r;

    pApp->Run();

    return 0;
}