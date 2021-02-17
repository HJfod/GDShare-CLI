#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <exception>
#include "rapidxml.hpp"
#include <ostream>

namespace gdshare {
    struct Result;
    struct Level;
    struct CCFile;
    struct CCLocalLevels;
    struct CCGameManager;

    /**
     * Returned by various functions to indicate
     * whether the function was succesful or not.
    */
    struct Result {
        /**
         * true if function was succesful, false if not.
        */
        bool OK;
        /**
         * Info about the result.
        */
        std::string info;
    };
    
    inline std::ostream& operator<< (std::ostream & strm, const Result & res) {
        return strm << "Result: " << (res.OK ? "OK, " : "Failed, ") << res.info;
    }

    namespace encoder {
        /**
         * Encode given vector with XOR
         * @param data The data to encode
         * @param key They key to use
        */
        std::vector<uint8_t> XOR(const std::vector<uint8_t> & data, int key);
        /**
         * Encode given vector with Base64
         * @param data The data to encode
        */
        std::vector<uint8_t> Base64(const std::vector<uint8_t> & data);
        /**
         * Encode given vector with GZip
         * @param data The data to encode
        */
        std::vector<uint8_t> GZip(const std::vector<uint8_t> & data);
    }

    namespace decoder {
        /**
         * Decode a string with XOR.
         * @param data The data to decode
         * @param key The key to use
        */
        std::string XOR(const std::string & data, int key);
        /**
         * Decode a string with Base 64.
         * @param data The data to decode
        */
        std::string Base64(const std::string & data);
        /**
         * Decode a string with GZip.
         * @param data The data to decode
        */
        std::string GZip(const std::string & data);

        // advanced versions for faster CCFile decoding

        /**
         * Decode a vector with XOR. To turn a string into a vector, use decoder::Convert.
         * @param data The data to decode
         * @param key The key to use
        */
        std::vector<uint8_t> XORX(const std::vector<uint8_t> & data, int key);
        /**
         * Decode a string with Base64. To turn a string into a vector, use decoder::Convert.
         * @param data The data to decode
        */
        std::vector<uint8_t> Base64X(const std::vector<uint8_t> & data);
        /**
         * Decode a string with GZip. To turn a string into a vector, use decoder::Convert.
         * @param data The data to decode
        */
        std::vector<uint8_t> GZipX(const std::vector<uint8_t> & data);

        // convert from string to vector

        /**
         * Turn a string into a vector of uint8_ts
         * @param string The string to convert
        */
        std::vector<uint8_t> Convert(const std::string & string);
        /**
         * Turn a vector of uint8_ts into a string
         * @param vector The vector to convert
        */
        std::string Convert(const std::vector<uint8_t> & vector);
    };

    namespace filetypes {
        /**
         * Modern filetype, equivalent to a .zip.
         * Further specifications in Standards.md
        */
        static constexpr const char* GDShare2 = "gmd2";

        /**
         * Legacy GDShare filetype standard
         * Further specifications in Standards.md
        */
        static constexpr const char* GDShare = "gmd";

        /**
         * Spu7nix's LvlShare filetype standard
         * Further specifications in Standards.md
        */
        static constexpr const char* LvlShare = "lvl";

        /**
         * Default export type
        */
        static constexpr const char* Default = GDShare;
    }

    /**
     * Represents a GD level.
    */
    struct Level {
        /**
         * Get a GD data key by a human-readable string.
         * List of strings coming soon in documentation
        */
        static std::unordered_map<std::string, std::string> Keys;

        /**
         * The XML document of the level.
         * READ ONLY
        */
        rapidxml::xml_node<>* xml;

        /**
         * Reset the level's XML document to an empty level.
        */
        void reset();

        /**
         * Get the value of a key in the level.
         * @param key The key to get
         * @returns Value of the key, or "" if key does not exist.
        */
        std::string key(std::string key);
        
        /**
         * Set the value of a key in the level. This method is chainable.
         * @param key The key to set
         * @param value The value to change to
         * @param type The type of the key. You rarely need to set this as
         * all default GD keys are figured out by the code.
         * @returns The this-pointer of the Level, meaning you can chain `key` methods.
        */
        Level* key(std::string key, std::string value, std::string type = "");
        
        /**
         * Get the name of the level.
         * @returns The level's name
        */
        std::string name();
        
        /**
         * Set the name of a level. This method is chainable.
         * @param name The name to change to
         * @returns The this-pointer of the Level, meaning you can chain this method.
        */
        Level*      name(std::string name);

        /**
         * Get the unencrypted data of a level.
         * @returns The level's data as a decrypted string
        */
        std::string data();
        
        /**
         * Set the data of a level. The data is passed as a raw level string, but is
         * automatically encoded by the method. This method is chainable.
         * @param data The data to change to
         * @returns The this-pointer of the Level, meaning you can chain this method.
        */
        Level*      data(std::string data);
        
        /**
         * Get the description of a level.
         * @returns The level's description
        */
        std::string description();
        
        /**
         * Set the description of a level. The description is encrypted with Base64 by
         * this method, as that is required by GD. This method is chainable.
         * @param desc The description to change to
         * @returns The this-pointer of the Level, meaning you can chain this method.
        */
        Level*      description(std::string desc);
        
        /**
         * Get the level's official song name / custom song ID.
         * @returns Level's song name if official, song ID if custom
        */
        std::string song();

        /**
         * Get the length of a level as a string (Tiny, Short, Medium, Long, XL)
         * @returns Level length as a string
        */
        std::string length();
        
        int objectCount();

        /**
         * Export a level as a file.
         * @param path The path to export to. If the path leads to a file,
         * the method will override that file. If it leads to a folder, it
         * will place the level file in that folder.
         * @param type The format of the exported file. Default is .gmd2.
         * @returns gdshare::Result
        */
        Result exportTo(std::string path = "", std::string type = filetypes::Default);

        /**
         * Import a level to a specified CCLocalLevels file.
         * Basically just an overload for CCLocalLevels::importLevel.
         * @param cclocal A CCLocalLevels object to import the level to
         * @returns gdshare::Result
        */
        Result importTo(CCLocalLevels* cclocal);

        /**
         * Generate generic default object string. Use if you want to completely override a level's data.
         * @returns String contains the object string
        */
        static std::string generateDefaultData();

        /**
         * Create an empty Level. A basic XML document for an empty unnamed level is initialized.
        */
        Level();

        /**
         * Create a Level from an XML document.
         * @param xml The node to create the level from. This should be the encompassing <d> tag.
        */
        Level(rapidxml::xml_node<>* xml);

        /**
         * Parse a level from a file.
         * @param path The path to the file.
         * @returns Parsed Level*.
        */
        static Level* load(const std::string & path);

        /**
         * Returns the contents of the Level in plain text.
         * @param intendation Whether to return the contents prettified.
         * @returns Contents of the Level as a string
        */
        std::string print(bool intendation = true);

        /**
         * Destroy a Level object.
        */
        ~Level();
    };

    /**
     * Parent class for CCLocalLevels and CCGameManager.
    */
    struct CCFile {
        /**
         * Path to the file. READ-ONLY!
        */
        std::string path;

        /**
         * Parsed XML content of the file. READ-ONLY!
        */
        rapidxml::xml_document<>* xml;

        /**
         * Initialize a new CCFile. Decodes and parses a .dat file.
         * @param path The path to a file compliant with GD CC files
         * @param callback Optional function for monitoring the progress of
         * decoding. First parameter is string info, second is percentage
         * decoded from 0-100.
        */
        Result init(std::string path, std::function<void (std::string, int)> callback = nullptr);

        /**
         * Decodes and parses a .dat file.
         * @param path The path to a file compliant with GD CC files
         * @param callback Optional function for monitoring the progress of
         * decoding. First parameter is string info, second is percentage
         * decoded from 0-100.
        */
        Result decode(std::string path, std::function<void(std::string, int)> callback = nullptr);

        /**
         * Save the CCFile along with any modifications you've made to it.
         * @param encode Whether to re-encode the data or leave it as a plain-text file.
         * @param callback Optional function for monitoring the progress of
         * saving. First parameter is string info, second is percentage
         * saved from 0-100.
        */
        Result save(bool encode = true, std::function<void(std::string, int)> callback = nullptr);

        /**
         * Returns the contents of the CCFile in plain text.
         * @param intendation Whether to return the contents prettified.
         * @returns Contents of the CCFile as a string
        */
        std::string print(bool intendation = true);

        /**
         * Destroy a CCFile. Overwritten by children.
        */
        virtual ~CCFile();
    };

    /**
     * Represents a CCLocalLevels.dat file.
    */
    struct CCLocalLevels : public CCFile {
        /**
         * Create a new CCLocalLevels object
         * and load vanilla GD's default CCLocalLevels.dat file for it.
         * @param callback Optional function for monitoring the progress of
         * the file's decoding. First parameter is string info, second is
         * percentage decoded from 0-100.
         * @throws std::runtime_error if unable to load the file.
        */
        CCLocalLevels(std::function<void (std::string, int)> callback = nullptr);

        /**
         * Create a new CCLocalLevels object and load a specified CCLocalLevels.dat file for it.
         * @param path The path to the CCLocalLevels.dat file.
         * @param callback Optional function for monitoring the progress of
         * the file's decoding. First parameter is string info, second is
         * percentage decoded from 0-100.
         * @throws std::runtime_error if unable to load the file.
        */
        CCLocalLevels(std::string path, std::function<void (std::string, int)> callback = nullptr);

        /**
         * Destroy a CCLocalLevels object.
        */
        ~CCLocalLevels() override;

        /**
         * Get a vector containing all the levels in the CCLocalLevels.
         * @returns Vector of pointers to Level
        */
        std::vector<Level*> getLevels();

        /**
         * Get a level by its name from the CCLocalLevels.
         * @param name The level's name.
         * @param casesensitive Whether to search for the level case-sensitive.
         * @returns Level* if found, nullptr if not.
        */
        Level* getLevel(std::string name, bool casesensitive = false);
        
        /**
         * Export a level by its name. Basically an overload for CCLocalLevels::getLevel and Level::exportTo.
         * @param name The level's name to export
         * @param path The path to export to. See Level::exportTo for details.
         * @param type The type of the export. See Level::exportTo for details.
         * @returns gdshare::Result
        */
        Result exportLevel(std::string name, std::string path = "", std::string type = filetypes::Default);

        /**
         * Export a level. Basically an overload for Level::exportTo.
         * @param level The Level to export
         * @param path The path to export to. See Level::exportTo for details.
         * @param type The type of the export. See Level::exportTo for details.
         * @returns gdshare::Result
        */
        Result exportLevel(Level* level, std::string path = "", std::string type = filetypes::Default);

        /**
         * Import a Level.
         * @param level The Level to import
         * @returns gdshare::Result
        */
        Result importLevel(Level* level);

        /**
         * Import a Level from a file, and return the imported Level*.
         * @param path The file to import
         * @returns The level that was imported, or nullptr if importing failed.
        */
        Level* importLevelC(const std::string & path);

        /**
         * Import a Level from a file.
         * @param path The file to import
         * @returns gdshare::Result
        */
        Result importLevel(const std::string & path);

        private:
            std::vector<Level*> $levels;
    };

    /**
     * Represents a CCGameManager.dat file.
    */
    struct CCGameManager : public CCFile {
        /**
         * Create a new CCGameManager object
         * and load vanilla GD's default CCGameManager.dat file for it.
         * @param callback Optional function for monitoring the progress of
         * the file's decoding. First parameter is string info, second is
         * percentage decoded from 0-100.
         * @throws std::runtime_error if unable to load the file.
        */
        CCGameManager(std::function<void (std::string, int)> = nullptr);

        /**
         * Create a new CCGameManager object and load a specified CCGameManager.dat file for it.
         * @param callback Optional function for monitoring the progress of
         * the file's decoding. First parameter is string info, second is
         * percentage decoded from 0-100.
         * @throws std::runtime_error if unable to load the file.
        */
        CCGameManager(std::string, std::function<void (std::string, int)> = nullptr);

        /**
         * Get the currently logged in player's username.
         * @returns The player's username, or "" if no one is logged in.
        */
        std::string username(void);

        /**
         * Get the currently logged in player's password.
         * @returns The player's password, or "" if no one is logged in.
        */
        std::string password(void);

        /**
         * Get a stat of the currently logged in player.
         * See documentation on info what stats you can get.
         * @param stat The stat to get. See documentation on info what stats you can get.
         * @returns The stat's value, or 0 if stat wasn't found.
        */
        int stat(std::string stat);

        /**
         * Get the value of a key in CCGameManager.
         * @param key The key which's value to get.
         * @returns The value of the key.
        */
        std::string val(std::string key);

        /**
         * Get the node of a key in CCGameManager.
         * @param key The key which to get.
         * @returns The corresponding XML node of the key.
        */
        rapidxml::xml_node<>* key(std::string key);

        CCGameManager* addSong(std::string id, std::string name);
    };

    namespace tools {
        enum Sorting {
            Normal,
            Name
        };
        void sortLevelList(std::vector<Level*>*, Sorting);
    }
}