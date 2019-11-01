#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Windows.h>

#ifdef Q_OS_WIN
#include <WinUser.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <gdiplus.h>
//#include <GdiPlusColor.h>
#include <QPushButton>

#endif
#pragma comment( lib, "user32.lib")
#pragma comment(lib, "dwmapi.lib")
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPushButton* pb = new QPushButton(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::mousePressEvent(QMouseEvent *event) {
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
#ifdef Q_OS_WIN
    window_borderless();
#endif
}

void MainWindow::window_borderless()
{
    if (isVisible())
    {
        //defaultStyle = (WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME)
        SetWindowLongPtr((HWND)winId(), GWL_STYLE, WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX );

        //windowShadow();

        SetWindowPos((HWND)winId(), 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
    }
}

void MainWindow::windowShadow()
{
    const MARGINS shadow={1, 1, 1, 1};
    HRESULT hr = S_OK;
    hr = DwmExtendFrameIntoClientArea((HWND)winId(), &shadow);
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)
    MSG* msg = reinterpret_cast<MSG*>(message);
    switch (msg->message)
    {
    case WM_NCCALCSIZE:
    {
        //this kills the window frame and title bar we added with
        //WS_THICKFRAME and WS_CAPTION
        *result = 0;
        return true;
        break;
    }
    case WM_NCHITTEST:
    {
        *result = 0;
        const LONG border_width = 8; //in pixels
        RECT winrect;
        GetWindowRect((HWND)winId(), &winrect);

        long x = GET_X_LPARAM(msg->lParam);
        long y = GET_Y_LPARAM(msg->lParam);

        bool resizeWidth = minimumWidth() != maximumWidth();
        bool resizeHeight = minimumHeight() != maximumHeight();

        if(resizeWidth)
        {
            //left border
            if (x >= winrect.left && x < winrect.left + border_width)
            {
                *result = HTLEFT;
            }
            //right border
            if (x < winrect.right && x >= winrect.right - border_width)
            {
                *result = HTRIGHT;
            }
        }
        if(resizeHeight)
        {
            //bottom border
            if (y < winrect.bottom && y >= winrect.bottom - border_width)
            {
                *result = HTBOTTOM;
            }
            //top border
            if (y >= winrect.top && y < winrect.top + border_width)
            {
                *result = HTTOP;
            }
        }
        if(resizeWidth && resizeHeight)
        {
            //bottom left corner
            if (x >= winrect.left && x < winrect.left + border_width &&
                    y < winrect.bottom && y >= winrect.bottom - border_width)
            {
                *result = HTBOTTOMLEFT;
            }
            //bottom right corner
            if (x < winrect.right && x >= winrect.right - border_width &&
                    y < winrect.bottom && y >= winrect.bottom - border_width)
            {
                *result = HTBOTTOMRIGHT;
            }
            //top left corner
            if (x >= winrect.left && x < winrect.left + border_width &&
                    y >= winrect.top && y < winrect.top + border_width)
            {
                *result = HTTOPLEFT;
            }
            //top right corner
            if (x < winrect.right && x >= winrect.right - border_width &&
                    y >= winrect.top && y < winrect.top + border_width)
            {
                *result = HTTOPRIGHT;
            }
        }

        //TODO: allow move?
        if(*result==0)
            *result = HTCAPTION ;

        return true;
        break;
    } //end case WM_NCHITTEST
    case WM_CLOSE:
    {
        return close();
        break;
    }
    default:
        return QMainWindow::nativeEvent(eventType,message,result);
    }
}
