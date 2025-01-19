#include "Strings.hpp"
#include "Console.hpp"
#include "Logger.hpp"
#include "Path.hpp"
#include <cstring>
#include <json-c/json.h>
#include <memory>
#include <switch.h>
#include <unordered_map>

namespace
{
    // This is a map of language codes along with their text representation. This is copy & pasted from JKSV rewrite.
    std::unordered_map<SetLanguage, std::string_view> s_FileMap = {{SetLanguage_JA, "JA.json"},
                                                                   {SetLanguage_ENUS, "ENUS.json"},
                                                                   {SetLanguage_FR, "FR.json"},
                                                                   {SetLanguage_DE, "DE.json"},
                                                                   {SetLanguage_IT, "IT.json"},
                                                                   {SetLanguage_ES, "ES.json"},
                                                                   {SetLanguage_ZHCN, "ZHCN.json"},
                                                                   {SetLanguage_KO, "KO.json"},
                                                                   {SetLanguage_NL, "NL.json"},
                                                                   {SetLanguage_PT, "PT.json"},
                                                                   {SetLanguage_RU, "RU.json"},
                                                                   {SetLanguage_ZHTW, "ZHTW.json"},
                                                                   {SetLanguage_ENGB, "ENGB.json"},
                                                                   {SetLanguage_FRCA, "FRCA.json"},
                                                                   {SetLanguage_ES419, "ES419.json"},
                                                                   {SetLanguage_ZHHANS, "ZHCN.json"},
                                                                   {SetLanguage_ZHHANT, "ZHTW.json"},
                                                                   {SetLanguage_PTBR, "PTBR.json"}};

    // This is the map of the few strings used by biggestDump.
    std::unordered_map<std::string, std::string> s_StringMap;
    // I didn''t feel like typing this for every set error.
    const char *DEFAULTING_TO_ENUS = "Defaulting to American English.";
} // namespace

static FsLib::Path GetFilePath(void)
{
    /*
        This is actually really useful since it can append from most standard C/C++ string types and return a valid path for stdio too, which
        we're using since I have no control over devkitPro libs and don't have time to port them to use FsLib.
    */
    FsLib::Path ReturnPath = "romfs:/";

    Result SetError = setInitialize();
    if (R_FAILED(SetError))
    {
        // This in English no matter what, because set failing means we have no way of determining the system's language.
        Console::Printf("*Error initializing Set service: 0x%X*. %s\n", SetError, DEFAULTING_TO_ENUS);
        // To do: Not repeat this for each error, but it works. Going to need FsLib update to accomplish.
        return ReturnPath += s_FileMap.at(SetLanguage_ENUS);
    }

    uint64_t LanguageCode = 0;
    SetError = setGetSystemLanguage(&LanguageCode);
    if (R_FAILED(SetError))
    {
        Console::Printf("*Error getting system language code: 0x%X. %s.\n", DEFAULTING_TO_ENUS);
        return ReturnPath += s_FileMap.at(SetLanguage_ENUS);
    }

    SetLanguage SystemLanguage;
    SetError = setMakeLanguage(LanguageCode, &SystemLanguage);
    if (R_FAILED(SetError))
    {
        Console::Printf("*Error making SetLanguage: 0x%X. %s.", SetError, DEFAULTING_TO_ENUS);
        return ReturnPath += s_FileMap.at(SetLanguage_ENUS);
    }
    return (ReturnPath += s_FileMap.at(SystemLanguage));
}

// From JKSV
static void ReplaceInString(std::string &Target, std::string_view Find, std::string_view Replace)
{
    size_t StringPosition = 0;
    if ((StringPosition = Target.find(Find.data(), StringPosition)) != Target.npos)
    {
        Target.replace(StringPosition, Find.length(), Replace.data());
    }
}

// From JKSV too.
static void ReplaceButtonsInString(std::string &Target)
{
    ReplaceInString(Target, "[A]", "\ue0e0");
    ReplaceInString(Target, "[B]", "\ue0e1");
    ReplaceInString(Target, "[X]", "\ue0e2");
    ReplaceInString(Target, "[Y]", "\ue0e3");
    ReplaceInString(Target, "[L]", "\ue0e4");
    ReplaceInString(Target, "[R]", "\ue0e5");
    ReplaceInString(Target, "[ZL]", "\ue0e6");
    ReplaceInString(Target, "[ZR]", "\ue0e7");
    ReplaceInString(Target, "[SL]", "\ue0e8");
    ReplaceInString(Target, "[SR]", "\ue0e9");
    ReplaceInString(Target, "[DPAD]", "\ue0ea");
    ReplaceInString(Target, "[DUP]", "\ue0eb");
    ReplaceInString(Target, "[DDOWN]", "\ue0ec");
    ReplaceInString(Target, "[DLEFT]", "\ue0ed");
    ReplaceInString(Target, "[DRIGHT]", "\ue0ee");
    ReplaceInString(Target, "[+]", "\ue0ef");
    ReplaceInString(Target, "[-]", "\ue0f0");
}

void Strings::Initialize(void)
{
    // This will return the path in the RomFS. FsLib can't support romfs for now, but its paths are useful.
    FsLib::Path JSONPath = GetFilePath();

    // This makes it so we don't have worry about freeing the json_object and memory leaking.
    std::unique_ptr<json_object, decltype(&json_object_put)> LanguageJSON(json_object_from_file(JSONPath.CString()), json_object_put);
    if (!LanguageJSON)
    {
        // To do: This isn't the best idea. There should probably be a backup plan here.
        return;
    }

    json_object_iterator StringIterator = json_object_iter_begin(LanguageJSON.get());
    json_object_iterator StringsEnd = json_object_iter_end(LanguageJSON.get());
    while (!json_object_iter_equal(&StringIterator, &StringsEnd))
    {
        const char *ObjectName = json_object_iter_peek_name(&StringIterator);
        json_object *ObjectValue = json_object_iter_peek_value(&StringIterator);

        // I'm hoping the hash will still be valid after the object is freed...
        s_StringMap[ObjectName] = json_object_get_string(ObjectValue);

        json_object_iter_next(&StringIterator);
    }

    for (auto &[StringKey, String] : s_StringMap)
    {
        ReplaceButtonsInString(String);
    }
}

const char *Strings::GetByName(std::string_view StringName)
{
    if (s_StringMap.find(StringName.data()) == s_StringMap.end())
    {
        return NULL;
    }
    return s_StringMap.at(StringName.data()).c_str();
}
