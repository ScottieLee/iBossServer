#ifndef FILELIST_H
#define FILELIST_H

#include <QObject>
#include <String>
#include <vector>
#include <QDir>

class FileNode;

class FileNode // A tree structure
{
public:
    explicit FileNode(std::string fileName):_childs(), _parent(0), _fileName(fileName), _isFolder(false) {}
    void addChild(FileNode* newChild){ _childs.push_back(newChild); }
    void setParent(FileNode* parent){ _parent = parent; }
	void setAbsPath(std::string path) { _absolutePath = path;}
    std::vector<FileNode*> getChilds(){ return _childs; }
    FileNode* getParent(){ return _parent; }
private:
    std::vector<FileNode*> _childs;
    FileNode*              _parent;
public:
    std::string            _fileName; // Could be a file or a folder
	std::string			   _absolutePath;
    bool                   _isFolder;

};


class FileList : public QObject
{
    Q_OBJECT
public:
    explicit FileList( std::string rootFolder = "", QObject *parent = 0);
	~FileList();
    //void setRootFolderPath(std::string);
	FileNode* getCurrentFileNode() { return _currentFileNode; }
    std::string listToACKFrame();
    void recursiveDown(int index);
    void backTrack();
	void toRootFolder();
	qint32 getNumberOfFolder() { return numberOfFolder; }
signals:

public slots:

protected:
	std::string         _rootFolderPath;
    FileNode*           _currentFileNode;
	FileNode*			_rootFileNode;
    //QStringList getList(); // get list of current filenode

	void FileList::traverse(FileNode* fileNode);
	virtual void formFileList(FileNode* fileNode, QDir dir); //  recursively construct the file tree
    virtual std::string _listToACKFrame();
	qint32 numberOfFolder;
	qint32 numberOfMusic;

private:
	void deleteTree(FileNode* root);
	// non copyable
	FileList& operator = (const FileList& rhs);
	FileList(const FileList& rhs);
};

#endif // FILELIST_H
