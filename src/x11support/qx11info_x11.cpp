/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Copyright (C) 2012 Richard Moore <rich@kde.org>
** Copyright (C) 2012 David Faure <david.faure@kdab.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//
// An implementation of QX11Info for Qt5. This code only provides the
// static methods of the QX11Info, not the methods for getting information
// about particular widgets or pixmaps.
//

#include "qx11info_x11.h"

#include <qpa/qplatformnativeinterface.h>
#include <qpa/qplatformwindow.h>
#include <qscreen.h>
#include <qdesktopwidget.h>
#include <qwindow.h>
#include <qapplication.h>
#include <xcb/xcb.h>

QT_BEGIN_NAMESPACE


/*!
    \class QX11Info
    \brief The QX11Info class provides information about the X display
    configuration.

    The class provides two APIs: a set of non-static functions that
    provide information about a specific widget or pixmap, and a set
    of static functions that provide the default information for the
    application.

    \warning This class is only available on X11. For querying
    per-screen information in a portable way, use QDesktopWidget.
*/

/*!
    Constructs an empty QX11Info object.
*/
QX11Info::QX11Info()
{
}

/*!
    Returns the horizontal resolution of the given \a screen in terms of the
    number of dots per inch.

    The \a screen argument is an X screen number. Be aware that if
    the user's system uses Xinerama (as opposed to traditional X11
    multiscreen), there is only one X screen. Use QDesktopWidget to
    query for information about Xinerama screens.

    \sa setAppDpiX(), appDpiY()
*/
int QX11Info::appDpiX(int screen)
{
    if (screen == -1) {
        const QScreen *scr = QGuiApplication::primaryScreen();
        if (!scr)
            return 75;
        return qRound(scr->logicalDotsPerInchX());
    }

    QList<QScreen *> screens = QGuiApplication::screens();
    if (screen >= screens.size())
        return 0;

    return screens[screen]->logicalDotsPerInchX();
}

/*!
    Returns the vertical resolution of the given \a screen in terms of the
    number of dots per inch.

    The \a screen argument is an X screen number. Be aware that if
    the user's system uses Xinerama (as opposed to traditional X11
    multiscreen), there is only one X screen. Use QDesktopWidget to
    query for information about Xinerama screens.

    \sa setAppDpiY(), appDpiX()
*/
int QX11Info::appDpiY(int screen)
{
    if (screen == -1) {
        const QScreen *scr = QGuiApplication::primaryScreen();
        if (!scr)
            return 75;
        return qRound(scr->logicalDotsPerInchY());
    }

    QList<QScreen *> screens = QGuiApplication::screens();
    if (screen > screens.size())
        return 0;

    return screens[screen]->logicalDotsPerInchY();
}

/*!
    Returns a handle for the applications root window on the given \a screen.

    The \a screen argument is an X screen number. Be aware that if
    the user's system uses Xinerama (as opposed to traditional X11
    multiscreen), there is only one X screen. Use QDesktopWidget to
    query for information about Xinerama screens.

    \sa QApplication::desktop()
*/
Qt::HANDLE QX11Info::appRootWindow(int screen)
{
    if (!qApp)
        return 0;
#if 0
    // This looks like it should work, but gives the wrong value.
    QDesktopWidget *desktop = QApplication::desktop();
    QWidget *screenWidget = desktop->screen(screen);
    QWindow *window = screenWidget->windowHandle();
#else
    Q_UNUSED(screen);

    QDesktopWidget *desktop = QApplication::desktop();
    QWindow *window = desktop->windowHandle();
#endif
    return Qt::HANDLE(window->winId());
}

/*!
    Returns the number of the screen where the application is being
    displayed.

    \sa display(), screen()
*/
int QX11Info::appScreen()
{
    if (!qApp)
        return 0;
    QDesktopWidget *desktop = QApplication::desktop();
    return desktop->primaryScreen();
}

/*!
    Returns the X11 time.

    \sa setAppTime(), appUserTime()
*/
unsigned long QX11Info::appTime()
{
    // ### TODO implement
    return 0L;
}

/*!
    Returns the X11 user time.

    \sa setAppUserTime(), appTime()
*/
unsigned long QX11Info::appUserTime()
{
    // ### TODO implement
    return 0L;
}

/*!
    Sets the X11 time to the value specified by \a time.

    \sa appTime(), setAppUserTime()
*/
void QX11Info::setAppTime(unsigned long time)
{
    // ### TODO implement
    Q_UNUSED(time);
}

/*!
    Sets the X11 user time as specified by \a time.

    \sa appUserTime(), setAppTime()
*/
void QX11Info::setAppUserTime(unsigned long time)
{
    if (!qApp)
        return;
    QPlatformNativeInterface *native = qApp->platformNativeInterface();

    QDesktopWidget *desktop = QApplication::desktop();
    QWindow *window = desktop->windowHandle();

    xcb_timestamp_t timestamp = uint32_t(time);
    QMetaObject::invokeMethod(native, "updateNetWmUserTime", Qt::DirectConnection,
                              Q_ARG(QWindow *,window), Q_ARG(xcb_timestamp_t, timestamp));
}

/*!
    Returns the default display for the application.

    \sa appScreen()
*/
Display *QX11Info::display()
{
    if (!qApp)
        return NULL;
    QPlatformNativeInterface *native = qApp->platformNativeInterface();

    void *display = native->nativeResourceForScreen(QByteArray("display"), QGuiApplication::primaryScreen());
    return reinterpret_cast<Display *>(display);
}

/*!
    Returns the default XCB connection for the application.

    \sa display()
*/
xcb_connection_t *QX11Info::connection()
{
    if (!qApp)
        return NULL;
    QPlatformNativeInterface *native = qApp->platformNativeInterface();

    void *connection = native->nativeResourceForWindow(QByteArray("connection"), 0);
    return reinterpret_cast<xcb_connection_t *>(connection);
}

QT_END_NAMESPACE