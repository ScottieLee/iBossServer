#ifndef MUSICFILELIST_H
#define MUSICFILELIST_H
#include "filelist.h"

class MusicFileList : public FileList
{
public:
    MusicFileList(std::string fileName);

protected:
    virtual void formFileList(FileNode* fileNode, QDir dir); //  recursively construct the file tree
    virtual std::string _listToACKFrame();

};

#endif // MUSICFILELIST_H
