#include "file_info.hh"

FileInfo::FileInfo() : size(0), isDir(false)
{
}

QDataStream &operator<<(QDataStream &out, const FileInfo &fileInfo)
{
	out << fileInfo.fileName << fileInfo.aTime << fileInfo.cTime
		<< fileInfo.mTime << fileInfo.size
		<< fileInfo.chunksLocation << fileInfo.isDir;
	return out;
}

QDataStream &operator>>(QDataStream &in, FileInfo &fileInfo)
{
	in >> fileInfo.fileName >> fileInfo.aTime >> fileInfo.cTime >>
		fileInfo.mTime >> fileInfo.size >> fileInfo.chunksLocation >>
		fileInfo.isDir;
	return in;
}

bool FileInfo::operator==(const FileInfo &other)
{
	if (this->fileName == other.fileName)
		return true;
	return false;
}