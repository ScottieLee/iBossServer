#include "musicfilelist.h"
#include "CodeDef.h"
#include <iostream>
MusicFileList::MusicFileList(std::string fileName):FileList(fileName)
{
	QDir rootDir(QString::fromStdString(_rootFolderPath));
	formFileList(_rootFileNode, rootDir);
}

void MusicFileList::formFileList(FileNode* fileNode, QDir currentDir)
{
    QDir::Filters filtDirs (QDir::AllDirs);
    QFileInfoList dirList = currentDir.entryInfoList(filtDirs|QDir::NoDotAndDotDot);
    for (int i = 0; i < dirList.size(); i++){
        FileNode* childNode = new FileNode(dirList[i].fileName().toUtf8().constData());
		childNode->setAbsPath(dirList[i].absoluteFilePath().toStdString());
		//std::cout << "Path: " << dirList[i].absoluteFilePath().toStdString() << std::endl;
        fileNode ->addChild(childNode);
        childNode->setParent(fileNode);
        childNode->_isFolder = true;
    }
    QStringList nameFilter;
    nameFilter << "*.mp3" ;
    QStringList fileList = currentDir.entryList(nameFilter,QDir::Files|QDir::NoDotAndDotDot );
	QFileInfoList fileInfoList = currentDir.entryInfoList(nameFilter,QDir::Files|QDir::NoDotAndDotDot);
    for (int i = 0; i < fileList.size(); i++){
        FileNode* childNode = new FileNode(fileList[i].toUtf8().constData());
		childNode->setAbsPath(fileInfoList[i].absoluteFilePath().toStdString());
		//std::cout << "Path: " << fileInfoList[i].absoluteFilePath().toStdString() << std::endl;
        fileNode ->addChild(childNode);
        childNode->setParent(fileNode);
    }
    for(int i = 0; i < dirList.size(); i++){
        QDir tDir(dirList[i].absoluteFilePath());
        formFileList(fileNode->getChilds()[i], tDir);
    }
}

/*
ML_ACK    | MUSICLIST_ACK | CODENONE | numberOfFolder | Folder1Name | Folder2Name | ... | numberOfMusic | Song1Name | Song2Name | ...|
                                                     4 Bytes         32 Bytes      32 Bytes             4 Bytes       32 Bytes    32 Bytes

*/
std::string MusicFileList::_listToACKFrame()
{
	numberOfMusic = 0;
	numberOfFolder = 0;
    char ML_ACK[4] = {MUSICLIST_ACK, CODENONE, 0x00, 0x00};
    std::vector<FileNode*> childs = _currentFileNode->getChilds();
    for(int v = 0; v < childs.size(); v++){
        if (childs[v]->_isFolder) numberOfFolder++;
        else numberOfMusic++;
    }
	//std::cout << "Number of folder is " << numberOfFolder << std::endl;
	//std::cout << "Number of music is " << numberOfMusic << std::endl;

    char* folderNameArray = new char[32*numberOfFolder];
    memset(folderNameArray, 0x00, 32*numberOfFolder);
    for(int v = 0; v < numberOfFolder; v++){
		//std::cout << "Foldername: " << childs[v]->_fileName << std::endl;
        char* thisFolder = (char*)(childs[v]->_fileName).data();
        memcpy(folderNameArray + v * 32, thisFolder, 32);
    }


    char* musicNameArray = new char[32*numberOfMusic];
    memset(musicNameArray, 0x00, 32*numberOfMusic);
    for(int v = 0; v < numberOfMusic; v++){
        char* thisSong = (char*)(childs[v + numberOfFolder]->_fileName).data();
        memcpy(musicNameArray + v * 32, thisSong, 32);
    }

    char* allData = new char[4 + 4 + 32*numberOfFolder + 4 + 32*numberOfMusic];
    memcpy(allData, ML_ACK, 4);
    memcpy(allData + 4, &numberOfFolder, 4);
    memcpy(allData + 8, folderNameArray, 32*numberOfFolder);
    memcpy(allData + 8 + 32*numberOfFolder, &numberOfMusic, 4);
    memcpy(allData + 8 + 32*numberOfFolder + 4, musicNameArray, 32*numberOfMusic);
	std::cout << "Generating ACK Frame .." << std::endl;
    std::string ACKFrame = std::string(allData, 4 + 4 + 32*numberOfFolder + 4 + 32*numberOfMusic);
    // delete
    delete[] folderNameArray;
    delete[] musicNameArray;
    delete[] allData;

    return ACKFrame;
}
