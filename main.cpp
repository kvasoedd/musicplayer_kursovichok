#include "musicplayer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("Fusion");
    MusicPlayer w;
    w.show();
    return a.exec();
}
