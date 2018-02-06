#include "widget.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    char lettre=' ';
    std::cout<<"argc : "<<argc<<std::endl;
    std::cout<<"argv[0] : "<<argv[0]<<std::endl;
    if (argc!=2)
    {
        std::cout<<"usage : "<<argv[0]<< " "<<"lettre de la couleur (j,v,b,m)"<<std::endl;
        //exit(0);
    }
    else
    {
        lettre=argv[1][0];
        std::cout<<"argv[1] : "<<argv[1];
        if ((lettre!='j') && (lettre!='v') && (lettre!='b') && (lettre!='m' )){
            std::cout<<"usage : "<<argv[0]<< " "<<"lettre de la couleur (r,v,b,m)"<<std::endl;
          //  exit(0);
        }
        else
        {


            //Widget w;
            Widget w(argv[1]);
            w.show();

            return a.exec();
        }
    }
    return EXIT_SUCCESS;
}
