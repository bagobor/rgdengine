#pragma once

#include "event/event.h"

class CWindowResize
{
public:
    CWindowResize(int x, int y): width(x),height(y) {}
    int	width;
    int height;
};

typedef void *WindowHandle;

namespace core
{
    class ITask;
    typedef boost::shared_ptr<ITask> PTask;

    class IApplication
    {
    protected:
		IApplication(){}

    public:
		virtual ~IApplication() {}
        virtual void			addTask(PTask pTask) = 0;
        virtual void			Run() = 0;
        virtual bool			update() = 0;
        virtual void			close() = 0;
        virtual WindowHandle	getWindowHandle() const = 0;

		static IApplication		*Create(const std::wstring& window_name = L"");
        static IApplication		*Create(const std::wstring& window_name, int Width, int Height, bool Fullscreen, bool resize_enable = true);
        static IApplication		*Create(WindowHandle hParentWindow);

        static IApplication		*Get();
    };
}