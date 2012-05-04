#include "zpFile.h"
#include "zpPackage.h"
#include <cassert>

namespace zp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
File::File(const Package* package, u64 offset, u32 size, u32 flag, u64 nameHash)
	: m_package(package)
	, m_offset(offset)
	, m_flag(flag)
	, m_size(size)
	, m_nameHash(nameHash)
	, m_readPos(0)
{
	assert(package != NULL);
	assert(package->m_stream != NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
File::~File()
{
	if (m_package->m_lastSeekFile == this)
	{
		m_package->m_lastSeekFile = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
u32 File::size() const
{
	return m_size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
u32 File::availableSize() const
{
	return m_package->getFileAvailableSize(m_nameHash);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
u32 File::flag() const
{
	return m_flag;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void File::seek(u32 pos)
{
	if (pos > m_size)
	{
		m_readPos = m_size;
	}
	else
	{
		m_readPos = pos;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
u32 File::tell() const
{
	return m_readPos;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
u32 File::read(u8* buffer, u32 size)
{
	//not preventing user from reading over available size here
	if (m_readPos + size > m_size)
	{
		size = m_size - m_readPos;
	}
	if (size == 0)
	{
		return 0;
	}
	if (m_package->m_lastSeekFile != this)
	{
		seekInPackage();
	}
	fread(buffer, size, 1, m_package->m_stream);
	m_readPos += size;
	return size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void File::seekInPackage()
{
	_fseeki64(m_package->m_stream, m_offset + m_readPos, SEEK_SET);
	m_package->m_lastSeekFile = this;
}

}
