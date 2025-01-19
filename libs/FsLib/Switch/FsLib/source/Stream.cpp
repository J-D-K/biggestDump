#include "Stream.hpp"

bool FsLib::Stream::IsOpen(void) const
{
    return m_IsOpen;
}

int64_t FsLib::Stream::Tell(void) const
{
    return m_Offset;
}

int64_t FsLib::Stream::GetSize(void) const
{
    return m_StreamSize;
}

bool FsLib::Stream::EndOfStream(void) const
{
    return m_Offset >= m_StreamSize;
}

void FsLib::Stream::Seek(int64_t Offset, uint8_t Origin)
{
    switch (Origin)
    {
        case Stream::Beginning:
        {
            m_Offset = 0 + Offset;
        }
        break;

        case Stream::Current:
        {
            m_Offset += Offset;
        }
        break;

        case Stream::End:
        {
            m_Offset = m_StreamSize + Offset;
        }
        break;
    }
    // Just in case.
    Stream::EnsureOffsetIsValid();
}

void FsLib::Stream::EnsureOffsetIsValid(void)
{
    if (m_Offset < 0)
    {
        m_Offset = 0;
    }
    else if (m_Offset >= m_StreamSize)
    {
        m_Offset = m_StreamSize - 1;
    }
}
