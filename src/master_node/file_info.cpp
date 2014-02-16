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