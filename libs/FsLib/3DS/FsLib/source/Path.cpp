#include "Path.hpp"
#include "String.hpp"
#include <algorithm>
#include <array>
#include <cstring>

namespace
{
    constexpr std::array<char16_t, 8> s_ForbiddenPathCharacters = {u'<', u'>', u':', u'\\', u'"', u'|', u'?', u'*'};
} // namespace

// Apparently C++ has no way of doing this in char_traits.
// C standard library has some pretty hard to decipher names.
const char16_t *U16StrPBrk(const char16_t *String)
{
    while (*String)
    {
        if (std::find(s_ForbiddenPathCharacters.begin(), s_ForbiddenPathCharacters.end(), *String) != s_ForbiddenPathCharacters.end())
        {
            return String;
        }
        ++String;
    }
    return nullptr;
}

// This is an all in one version of what Switch does. Passing nullptr as PathBegin skips trimming the beginning.
void GetTrimmedPath(const char16_t *Path, const char16_t **PathBegin, size_t &PathLength)
{
    // This will skip over beginning slashes.
    if (PathBegin)
    {
        while (*Path == u'/')
        {
            ++Path;
        }
        *PathBegin = Path;
    }

    PathLength = std::char_traits<char16_t>::length(Path);
    while (PathLength > 0 && Path[PathLength - 1] == u'/')
    {
        --PathLength;
    }
}

FsLib::Path::Path(const FsLib::Path &P)
{
    *this = P;
}

FsLib::Path::Path(const char16_t *P)
{
    *this = P;
}

FsLib::Path::Path(const uint16_t *P)
{
    *this = P;
}

FsLib::Path::Path(const std::u16string &P)
{
    *this = P;
}

FsLib::Path::Path(std::u16string_view P)
{
    *this = P;
}

FsLib::Path::~Path()
{
    Path::FreePath();
}

bool FsLib::Path::IsValid(void) const
{
    return m_Path != nullptr && m_DeviceEnd != nullptr && std::char_traits<char16_t>::length(m_Path) > 0 &&
           U16StrPBrk(m_DeviceEnd + 1) == nullptr;
}

FsLib::Path FsLib::Path::SubPath(size_t PathLength) const
{
    if (PathLength >= m_PathLength)
    {
        PathLength = m_PathLength;
    }

    FsLib::Path NewPath;
    if (NewPath.AllocatePath(m_PathSize))
    {
        std::memcpy(NewPath.m_Path, m_Path, PathLength * sizeof(char16_t));
        NewPath.m_DeviceEnd = std::char_traits<char16_t>::find(NewPath.m_Path, m_PathLength, u':');
        NewPath.m_PathLength = m_PathLength;
    }
    return NewPath;
}

size_t FsLib::Path::FindFirstOf(char16_t Character) const
{
    for (size_t i = 0; i < m_PathLength; i++)
    {
        if (m_Path[i] == Character)
        {
            return i;
        }
    }
    return Path::NotFound;
}

size_t FsLib::Path::FindFirstOf(char16_t Character, size_t Begin) const
{
    if (Begin >= m_PathLength)
    {
        return Path::NotFound;
    }

    for (size_t i = Begin; i < m_PathLength; i++)
    {
        if (m_Path[i] == Character)
        {
            return i;
        }
    }
    return Path::NotFound;
}

size_t FsLib::Path::FindLastOf(char16_t Character) const
{
    for (size_t i = m_PathLength; i > 0; i--)
    {
        if (m_Path[i] == Character)
        {
            return i;
        }
    }
    return Path::NotFound;
}

size_t FsLib::Path::FindLastOf(char16_t Character, size_t Begin) const
{
    if (Begin > m_PathLength)
    {
        Begin = m_PathLength;
    }

    for (size_t i = Begin; i > m_PathLength; i--)
    {
        if (m_Path[i] == Character)
        {
            return i;
        }
    }
    return Path::NotFound;
}

const char16_t *FsLib::Path::CString(void) const
{
    return m_Path;
}

std::u16string_view FsLib::Path::GetDevice(void) const
{
    return std::u16string_view(m_Path, m_DeviceEnd - m_Path);
}

std::u16string_view FsLib::Path::GetFileName(void) const
{
    size_t FileNameBegin = Path::FindLastOf(u'/');
    size_t FileNameEnd = Path::FindLastOf(u'.');
    if (FileNameBegin == Path::NotFound || FileNameEnd == Path::NotFound)
    {
        return std::u16string_view(u"");
    }
    ++FileNameBegin;
    --FileNameEnd;
    return std::u16string_view(&m_Path[FileNameBegin], FileNameEnd - FileNameBegin);
}

std::u16string_view FsLib::Path::GetExtension(void) const
{
    size_t ExtensionBegin = Path::FindLastOf(u'.');
    if (ExtensionBegin == Path::NotFound)
    {
        return std::u16string_view(u"");
    }
    return std::u16string_view(&m_Path[ExtensionBegin] + 1, m_PathLength);
}

FS_Path FsLib::Path::GetPath(void) const
{
    return {PATH_UTF16, (std::char_traits<char16_t>::length(m_DeviceEnd + 1) * sizeof(char16_t)) + sizeof(char16_t), m_DeviceEnd + 1};
}

size_t FsLib::Path::GetLength(void) const
{
    return m_PathLength;
}

FsLib::Path &FsLib::Path::operator=(const FsLib::Path &P)
{
    if (!Path::AllocatePath(P.m_PathSize))
    {
        // To do: Something with errors.
        return *this;
    }

    // Just copy the path.
    std::memcpy(m_Path, P.m_Path, P.m_PathLength * sizeof(char16_t));
    // These need to point to this path instead of the other.
    m_PathSize = P.m_PathSize;
    m_PathLength = P.m_PathLength;
    m_DeviceEnd = std::char_traits<char16_t>::find(m_Path, m_PathLength, u':');

    return *this;
}

FsLib::Path &FsLib::Path::operator=(const char16_t *P)
{
    // Gotta calculate how much memory to allocate for path.
    size_t PathLength = std::char_traits<char16_t>::length(P);
    m_DeviceEnd = std::char_traits<char16_t>::find(P, PathLength, u':');
    if (!m_DeviceEnd)
    {
        // Should do something here.
        return *this;
    }

    m_PathSize = FsLib::MAX_PATH + ((m_DeviceEnd - P) + 1);
    if (!Path::AllocatePath(m_PathSize))
    {
        return *this;
    }

    const char16_t *PathBegin = nullptr;
    size_t SubPathLength = 0;
    GetTrimmedPath(m_DeviceEnd + 1, &PathBegin, SubPathLength);

    // Copy device first.
    std::memcpy(m_Path, P, ((m_DeviceEnd - P) + 2) * sizeof(char16_t));
    // Copy the rest, but trimmed.
    std::memcpy(&m_Path[std::char_traits<char16_t>::length(m_Path)], PathBegin, SubPathLength * sizeof(char16_t));
    m_PathLength = std::char_traits<char16_t>::length(m_Path);
    m_DeviceEnd = std::char_traits<char16_t>::find(m_Path, m_PathLength, u':');

    return *this;
}

FsLib::Path &FsLib::Path::operator=(const uint16_t *P)
{
    return *this = reinterpret_cast<const char16_t *>(P);
}

FsLib::Path &FsLib::Path::operator=(const std::u16string &P)
{
    return *this = P.c_str();
}

FsLib::Path &FsLib::Path::operator=(std::u16string_view P)
{
    return *this = P.data();
}

FsLib::Path &FsLib::Path::operator/=(const char16_t *P)
{
    const char16_t *PathBegin = nullptr;
    size_t PathLength = 0;
    GetTrimmedPath(P, &PathBegin, PathLength);

    if ((m_PathLength + PathLength) + 1 >= m_PathSize)
    {
        return *this;
    }

    if (m_Path[m_PathLength - 1] != u'/')
    {
        m_Path[m_PathLength++] = u'/';
    }

    std::memcpy(&m_Path[m_PathLength], PathBegin, PathLength * sizeof(char16_t));

    m_PathLength += PathLength;

    return *this;
}

FsLib::Path &FsLib::Path::operator/=(const uint16_t *P)
{
    return *this /= reinterpret_cast<const char16_t *>(P);
}

FsLib::Path &FsLib::Path::operator/=(const std::u16string &P)
{
    return *this /= P.c_str();
}

FsLib::Path &FsLib::Path::operator/=(std::u16string_view P)
{
    return *this /= P.data();
}

FsLib::Path &FsLib::Path::operator+=(const char16_t *P)
{
    size_t PathLength = std::char_traits<char16_t>::length(P);
    if (m_PathLength + PathLength >= m_PathSize)
    {
        return *this;
    }

    std::memcpy(&m_Path[m_PathLength], P, PathLength * sizeof(char16_t));

    m_PathLength += PathLength;

    return *this;
}

FsLib::Path &FsLib::Path::operator+=(const uint16_t *P)
{
    return *this += reinterpret_cast<const char16_t *>(P);
}

FsLib::Path &FsLib::Path::operator+=(const std::u16string &P)
{
    return *this += P.c_str();
}

FsLib::Path &FsLib::Path::operator+=(std::u16string_view P)
{
    return *this += P.data();
}

bool FsLib::Path::AllocatePath(uint16_t PathSize)
{
    Path::FreePath();
    m_Path = new (std::nothrow) char16_t[PathSize];
    if (!m_Path)
    {
        return false;
    }
    std::memset(m_Path, 0x00, PathSize * sizeof(char16_t));
    return true;
}

void FsLib::Path::FreePath(void)
{
    if (m_Path)
    {
        delete[] m_Path;
    }
}

FsLib::Path FsLib::operator/(const FsLib::Path &Path1, const char16_t *Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath /= Path2;
    return NewPath;
}

FsLib::Path FsLib::operator/(const FsLib::Path &Path1, const uint16_t *Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath /= Path2;
    return NewPath;
}

FsLib::Path FsLib::operator/(const FsLib::Path &Path1, const std::u16string &Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath /= Path2;
    return NewPath;
}

FsLib::Path FsLib::operator/(const FsLib::Path &Path1, std::u16string_view Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath /= Path2;
    return NewPath;
}

FsLib::Path FsLib::operator+(const FsLib::Path &Path1, const char16_t *Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath += Path2;
    return NewPath;
}

FsLib::Path FsLib::operator+(const FsLib::Path &Path1, const uint16_t *Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath += Path2;
    return NewPath;
}

FsLib::Path FsLib::operator+(const FsLib::Path &Path1, const std::u16string &Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath += Path2;
    return NewPath;
}

FsLib::Path FsLib::operator+(const FsLib::Path &Path1, std::u16string_view Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath += Path2;
    return NewPath;
}
