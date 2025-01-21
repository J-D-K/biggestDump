#include "strings.hpp"
#include "console.hpp"
#include "fslib.hpp"
#include "logger.hpp"
#include <cstring>
#include <json-c/json.h>
#include <memory>
#include <switch.h>
#include <unordered_map>

namespace
{
    // This is a map of language codes along with their text representation. This is copy & pasted from JKSV rewrite.
    std::unordered_map<SetLanguage, std::string_view> s_fileMap = {{SetLanguage_JA, "JA.json"},
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
    std::unordered_map<std::string, std::string> s_stringMap;
    // I didn''t feel like typing this for every set error.
    const char *DEFAULTING_TO_ENUS = "Defaulting to American English.";
} // namespace

static fslib::Path getFilePath(void)
{
    fslib::Path returnPath = "romfs:/";

    if (R_FAILED(setInitialize()))
    {
        Console::printf("*Error initializing set service*. %s.\n", DEFAULTING_TO_ENUS);
        return returnPath + s_fileMap.at(SetLanguage_ENUS);
    }

    uint64_t languageCode = 0;
    if (R_FAILED(setGetSystemLanguage(&languageCode)))
    {
        Console::printf("*Error getting system language code*. %s\n", DEFAULTING_TO_ENUS);
        return returnPath + s_fileMap.at(SetLanguage_ENUS);
    }

    SetLanguage systemLanguage;
    if (R_FAILED(setMakeLanguage(languageCode, &systemLanguage)))
    {
        Console::printf("*Error making set language*. %s.\n", DEFAULTING_TO_ENUS);
        return returnPath + s_fileMap.at(SetLanguage_ENUS);
    }

    return returnPath + s_fileMap.at(systemLanguage);
}

// From JKSV
static void replaceInString(std::string &target, std::string_view find, std::string_view replace)
{
    size_t stringPosition = 0;
    while ((stringPosition = target.find(find.data(), stringPosition)) != target.npos)
    {
        target.replace(stringPosition, find.length(), replace);
    }
}

// From JKSV too.
static void replaceButtonsInString(std::string &Target)
{
    replaceInString(Target, "[A]", "\ue0e0");
    replaceInString(Target, "[B]", "\ue0e1");
    replaceInString(Target, "[X]", "\ue0e2");
    replaceInString(Target, "[Y]", "\ue0e3");
    replaceInString(Target, "[L]", "\ue0e4");
    replaceInString(Target, "[R]", "\ue0e5");
    replaceInString(Target, "[ZL]", "\ue0e6");
    replaceInString(Target, "[ZR]", "\ue0e7");
    replaceInString(Target, "[SL]", "\ue0e8");
    replaceInString(Target, "[SR]", "\ue0e9");
    replaceInString(Target, "[DPAD]", "\ue0ea");
    replaceInString(Target, "[DUP]", "\ue0eb");
    replaceInString(Target, "[DDOWN]", "\ue0ec");
    replaceInString(Target, "[DLEFT]", "\ue0ed");
    replaceInString(Target, "[DRIGHT]", "\ue0ee");
    replaceInString(Target, "[+]", "\ue0ef");
    replaceInString(Target, "[-]", "\ue0f0");
}

void strings::initialize(void)
{
    // This will return the path in the RomFS. FsLib can't support romfs for now, but its paths are useful.
    fslib::Path jsonPath = getFilePath();

    // This makes it so we don't have worry about freeing the json_object and memory leaking.
    std::unique_ptr<json_object, decltype(&json_object_put)> langJson(json_object_from_file(jsonPath.cString()), json_object_put);
    if (!langJson)
    {
        // To do: This isn't the best idea. There should probably be a backup plan here.
        return;
    }

    // Iterate through json file and map the strings.
    json_object_iterator stringIterator = json_object_iter_begin(langJson.get());
    json_object_iterator stringEnd = json_object_iter_end(langJson.get());
    while (!json_object_iter_equal(&stringIterator, &stringEnd))
    {
        const char *objectName = json_object_iter_peek_name(&stringIterator);
        json_object *objectValue = json_object_iter_peek_value(&stringIterator);

        s_stringMap[objectName] = json_object_get_string(objectValue);

        json_object_iter_next(&stringIterator);
    }

    // Loop through and replace the button strings.
    for (auto &[stringKey, string] : s_stringMap)
    {
        replaceButtonsInString(string);
    }
}

const char *strings::getByName(std::string_view stringName)
{
    if (s_stringMap.find(stringName.data()) == s_stringMap.end())
    {
        return nullptr;
    }
    return s_stringMap.at(stringName.data()).c_str();
}
