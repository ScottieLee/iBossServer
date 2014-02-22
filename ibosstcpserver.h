#ifndef IBOSSTCPSERVER_H
#define IBOSSTCPSERVER_H

#include <QtNetwork>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <String>
#include "musicfilelist.h"
#include <QMediaPlayer>
#include <QURL>

enum ServerState
{
	UNAUTHORIZED,
	BEFORE_AUTHORIZED, // Before send authorized msg to iphone
	AUTHORIZED,
	MUSICLIST_SENT
};

class iBossTCPServer: public QObject
{
Q_OBJECT
public:
  iBossTCPServer(QObject * parent = 0, std::string key = std::string("000000"));
  ~iBossTCPServer();
public slots:
  void acceptConnection();
  void startRead();
  void clientConnected();
  void clientDisconnected();
  void bytesBeenWritten(qint64);

private:
  std::string _key;
  short _identityCode;
  FileList* _musicList;
  std::string _musicRootFolder;
  QTcpServer _server;
  QTcpSocket* _clientSocket;
  ServerState _serverState;
  QMediaPlayer* _player;
  void readStateMachine(int nbytesRead, char* frame);
  bool checkIdentity();
  static int defaultVolume;
};
#endif // IBOSSTCPSERVER_H
