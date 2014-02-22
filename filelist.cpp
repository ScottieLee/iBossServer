#include "filelist.h"
#include <QStringList>
#include <cassert>
#include <iostream>

FileList::FileList(std::string rootFolderPath, QObject *parent) : _rootFolderPath(rootFolderPath), numberOfFolder(0),numberOfMusic(0),
    QObject(parent)
{
	QDir rootDir(QString::fromStdString(_rootFolderPath));
	_rootFileNode = new FileNode(_rootFolderPath);
	_rootFileNode->setAbsPath(_rootFolderPath);
	//formFileList(_rootFileNode, rootDir);
	_currentFileNode = _rootFileNode;
	//traverse(_currentFileNode);
}

FileList::~FileList()
{
	deleteTree(_rootFileNode);
}

void FileList::deleteTree(FileNode* root)
{
	for (int v = 0; v < root->getChilds().size(); v++){
		deleteTree(root->getChilds()[v]);
	}
	delete root;
}

void FileList::traverse(FileNode* fileNode)
{
	std::cout << "Current node: " << (fileNode->_fileName) << std::endl;
	for(int v = 0; v < fileNode->getChilds().size(); v++){
		traverse(fileNode->getChilds()[v]);
	}
}

void FileList::formFileList(FileNode* fileNode, QDir currentDir)
{
	//QStringList nameFilter;
	//nameFilter << "*.mp3";
	//std::cout << "Enter function currendir is " << fileNode->_fileName << std::endl;
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
	QStringList fileList = currentDir.entryList(QDir::Files|QDir::NoDotAndDotDot );
	QFileInfoList fileInfoList = currentDir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot );
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

void FileList::recursiveDown(int index)
{

	std::vector<FileNode*> childs = _currentFileNode->getChilds();
	assert(childs.size() >= index);
	_currentFileNode = childs[index];
	std::cout << "Forward" << std::endl;
	//traverse(_currentFileNode);
}

void FileList::backTrack()
{
	_currentFileNode = _currentFileNode->getParent();
	std::cout << "Back" << std::endl;
	//traverse(_currentFileNode);
}

std::string FileList::listToACKFrame()
{
	return _listToACKFrame();
}

std::string FileList::_listToACKFrame() { return 0; }

void FileList::toRootFolder()
{
	_currentFileNode = _rootFileNode;
}
