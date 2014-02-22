#include <cassert>
#include <iostream>
#include "ibosstcpserver.h"
#include "CodeDef.h"

using namespace std;

int iBossTCPServer::defaultVolume = 25;

iBossTCPServer::iBossTCPServer(QObject* parent, std::string key): QObject(parent),_key(key), _serverState(UNAUTHORIZED)
{
	_musicRootFolder = std::string("F:/Muzik/");
	_musicList = new MusicFileList(_musicRootFolder);
	connect(&_server, SIGNAL(newConnection()),this, SLOT(acceptConnection()));

	_server.listen(QHostAddress::Any, 2048);
	// Generate an Identity Code
	_identityCode = rand();
	_player = new QMediaPlayer();
	_player->setVolume(defaultVolume);

	std::cout << "Listening" << std::endl;
}

iBossTCPServer::~iBossTCPServer()
{
	_clientSocket->close();
	_server.close();
	delete _musicList;
	delete _player;
}

//SLOT
void iBossTCPServer::acceptConnection()
{
	_clientSocket = _server.nextPendingConnection();

	std::cout << "Accept connection, _identity code is  " << _identityCode << std::endl;
	// Configuring socket signals
	connect(_clientSocket, SIGNAL(connected()),this, SLOT(clientConnected()));
	connect(_clientSocket, SIGNAL(disconnected()),this, SLOT(clientDisconnected()));
	connect(_clientSocket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesBeenWritten(qint64)));
	connect(_clientSocket, SIGNAL(readyRead()), this, SLOT(startRead()));
	//_clientSocket->write("Hellooooo");
}
//SLOT
void iBossTCPServer::startRead()
{
	std::cout << "Start Read " << std::endl;
	char readbuffer[1024] = {0};
	int nbytesRead = _clientSocket -> read(readbuffer, _clientSocket->bytesAvailable());
	readStateMachine(nbytesRead, readbuffer);
  //_clientSocket -> close();
}

//SLOT
void iBossTCPServer::bytesBeenWritten(qint64 len)
{
	switch (_serverState)
	{
		case UNAUTHORIZED:
		{
			// REJ has sent to iPhone
			std::cout << "REJ sent" << len << std::endl;
			break;
		}
		case BEFORE_AUTHORIZED:
		{
			std::cout << "ACC sent" <<  len << std::endl;
			_serverState = AUTHORIZED; // ACC has sent to iPhone, Switch to AUTHORIZED state
			break;
		}

		default: break;
	}

}

void iBossTCPServer::clientConnected()
{
	std::cout << "Client connetcted. " << std::endl;
}

void iBossTCPServer::clientDisconnected()
{
	std::cout << "Client disconnected. " << std::endl;
	_serverState = UNAUTHORIZED;
	disconnect(_clientSocket, SIGNAL(connected()),this, SLOT(clientConnected()));
	disconnect(_clientSocket, SIGNAL(disconnected()),this, SLOT(clientDisconnected()));
	disconnect(_clientSocket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesBeenWritten(qint64)));
	disconnect(_clientSocket, SIGNAL(readyRead()), this, SLOT(startRead()));
	_clientSocket->close();
	_player->stop();
	_player->setVolume(defaultVolume);
}

// Main logic for read
void iBossTCPServer::readStateMachine(int nbytesRead, char* frame)
{
	std::cout << "Enter Read State Machine  " << _serverState << std::endl;
	// Get data
	std::string data = std::string(frame + DATA_OFFSET, nbytesRead - DATA_OFFSET); 
	switch (_serverState)
	{
		case UNAUTHORIZED:
		{
            if (frame[1] != AUTHORIZE_REQUEST){ //This is a "noise"

				std::cout << "This is a noise!" << std::endl;
				break;
			}

			if (data == _key) { 
				std::cout << "Key is matched " << std::endl;
				_serverState = BEFORE_AUTHORIZED; // Key is matched, Switch to BEFORE_AUTHORIZED state
				//FIXME Log this client that succeed to match
				// Send authorized msg
				char ACC[4] = {AUTHORIZE_ACCEPTED, CODENONE, 0x00, 0x00};
				memcpy(ACC+2, &_identityCode, sizeof(short));
				_clientSocket->write(ACC, DATA_OFFSET);

				// everty time the iphone login, we reset the music list to the root list
				_musicList->toRootFolder();
			} else {
				std::cout << "Length of data is " << data.length() << std::endl; 
				//FIXME Log this client that failed to match
				// Still at UNAUTHORIZED state
				char REJ[4] = {AUTHORIZE_REJECTED, CODENONE, 0x00, 0x00};
				_clientSocket->write(REJ, DATA_OFFSET);
			}
			break;
		}
		case AUTHORIZED:
		{
			short receivedIdentity;
			memcpy(&receivedIdentity, frame + 2, sizeof(short));
            if ( _identityCode == receivedIdentity){// identityCode Matched

				switch (frame[1]){
					case MUSICLIST_REQUEST: {
						std::string ACKFrame = _musicList->listToACKFrame();
						_clientSocket->write(ACKFrame.data(), ACKFrame.length());
						break;
					}
					case MUSICLIST_FORWARD:{
						int index;
						memcpy(&index, data.data(), sizeof(int));
						_musicList->recursiveDown(index);
						break;
					}
					case MUSICLIST_BACK:{
						_musicList->backTrack();
						break;
					}
					case MUSIC_PLAY:{
						int songIndex;
						memcpy(&songIndex, frame + 4, sizeof(int));
						int numberOfFolder = _musicList->getNumberOfFolder();
						std::string songAbsPath = _musicList->getCurrentFileNode()->getChilds()[numberOfFolder + songIndex]->_absolutePath;
						_player->stop();
						_player->setMedia(QUrl::fromLocalFile(QString::fromStdString(songAbsPath)));
						
						_player->play();
						std::cout << "Playing " << std::endl;
						break;
					}
					case MUSIC_PAUSE:{
						_player->pause();
						break;
					}
					case MUSIC_RESUME:{
						//std::cout << (_player->media()) << std::endl;
						_player->play();
						break;
					}
					case MUSIC_VOLUME:{

						int volume;
						memcpy(&volume, frame + 4, sizeof(int));
						volume = (volume>100)? 100: volume;
						std::cout << "Changing volume to " << volume << std::endl;
						_player->setVolume(volume);
						break;
					}
					case SHUTDOWN_NOW:{
						QProcess::startDetached("shutdown -s -f -t 00");
						break;
					}
				}
			}// if authorized 
			break;
		} // case AUTHORIZED
		default: break;
	}
}
