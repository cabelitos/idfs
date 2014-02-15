#include "file_info.hh"

QDataStream &operator<<(QDataStream &out, const FileInfo &fileInfo)
{
	out << fileInfo.getFileName() << fileInfo.getATime() << fileInfo.getCTime()
		<< fileInfo.getMTime() << fileInfo.getSize()
		<< fileInfo.getChunksLocation() << fileInfo.isDir();
	return out;
}

QDataStream &operator>>(QDataStream &in, FileInfo &fileInfo)
{
	QDateTime aTime, cTime, mTime;
	QString name;
	qint64 size;
	QList<QPair<QString, QString> > chunksLocation;
	bool isDir;

	in >> name >> aTime >> cTime >> mTime >> size >> chunksLocation >> isDir;
	fileInfo.setFileName(name);
	fileInfo.setATime(aTime);
	fileInfo.setCTime(cTime);
	fileInfo.setMTime(mTime);
	fileInfo.setSize(size);
	fileInfo.setChunksLocation(chunksLocation);
	fileInfo.setIsDir(isDir);
	return in;
}

FileInfo::FileInfo() : _size(0), _isDir(false)
{
}

FileInfo::~FileInfo()
{
}

void FileInfo::setIsDir(bool isDir)
{
	this->_isDir = isDir;
}

void FileInfo::setChunksLocation(
	const QList<QPair<QString, QString> > &chunksLocation)
{
	this->_chunksLocation = chunksLocation;
}

void FileInfo::setFileName(const QString &fileName)
{
	this->_fileName = fileName;
}

void FileInfo::setATime(const QDateTime &aTime)
{
	this->_aTime = aTime;
}

void FileInfo::setCTime(const QDateTime &cTime)
{
	this->_cTime = cTime;
}

void FileInfo::setMTime(const QDateTime &mTime)
{
	this->_mTime = mTime;
}

void FileInfo::setSize(qint64 size)
{
	this->_size = size;
}

void FileInfo::addChunkLocation(const QString &slaveName,
	const QString &chunkName)
{
	this->_chunksLocation.
		append(QPair<QString,QString>(slaveName, chunkName));
}

bool FileInfo::isDir() const
{
	return this->_isDir;
}

qint64 FileInfo::getSize() const
{
	return this->_size;
}

const QDateTime &FileInfo::getATime() const
{
	return this->_aTime;
}

const QDateTime &FileInfo::getCTime() const
{
	return this->_cTime;
}

const QDateTime &FileInfo::getMTime() const
{
	return this->_mTime;
}

const QString &FileInfo::getFileName() const
{
	return this->_fileName;
}

const QList<QPair<QString, QString> > &FileInfo::getChunksLocation() const
{
	return this->_chunksLocation;
}