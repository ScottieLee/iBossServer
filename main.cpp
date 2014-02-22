#include <QApplication>
#include "iBossTCPServer.h"
#define Test 1
#if Test
#include "musicfilelist.h"
#include <iostream>

#endif
int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  iBossTCPServer server;
//  MusicFileList* list = new MusicFileList("F:/test");
//  list->recursiveDown(0);
//  list->recursiveDown(0);
//  list->backTrack();
//  list->backTrack();

  //QMediaPlayer *player = new QMediaPlayer();
  //std::string lof("F:/test/Bruno.mp3");

  //player->setMedia(QUrl::fromLocalFile(QString::fromStdString(lof)));
  //player->setVolume(90);
  //player->play();
  //::cout << "Playing " << std::endl;
  
  return app.exec();
}
