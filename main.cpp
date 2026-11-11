#include "stdafx.h"
#include "mynotepad.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    mynotepad window;
    window.show();
    return app.exec();
}
