#include "mainwindow.h"

#include <QApplication>



//#include "myRISCVSim.h"
#include <stdio.h>
#include <stdlib.h>




int main(int argc, char **argv)
{
//    freopen("output.txt","w",stdout);
    if(argc < 2) {
      printf("Incorrect number of arguments. Please invoke the simulator \n\t./myRISCVSim <input mem file> \n");
      exit(1);
    }
    reset_proc();

//    //load the program memory
    load_program_memory(argv[1]);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
