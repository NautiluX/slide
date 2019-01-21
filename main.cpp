#include "mainwindow.h"
#include "imageselector.h"
#include <QApplication>
#include <iostream>
#include <sys/file.h>
#include <errno.h>

int main(int argc, char *argv[])
{

    int pid_file = open("/tmp/slide.pid", O_CREAT | O_RDWR, 0666);
    int rc = flock(pid_file, LOCK_EX | LOCK_NB);
    if(rc) {
        if(EWOULDBLOCK == errno) {
            std::cout << "Already running. Exiting" << std::endl;
            return 0;
        }
    }
    if (argc < 2) {
        std::cout << "Error: Path expected." << std::endl;
        return 1;
    }
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    ImageSelector is(w, 30000, argv[1]);
    is.start();
    return a.exec();
}
