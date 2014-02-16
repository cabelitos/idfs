#include "file_info.hh"

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
	if (this->fileName == other.fileName && this->aTime == other.aTime &&
		this->mTime == other.mTime && this->cTime == other.cTime &&
		this->isDir == other.isDir && this->size == other.size &&
		this->chunksLocation == other.chunksLocation)
		return true;
	return false;
}