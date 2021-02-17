#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <cctype>
#include <locale>
#include <sstream>
#include <algorithm>
#include <map>
#include <Windows.h>
#include "gdshare.hpp"

using namespace gdshare;

namespace gdscli {
    // trim from start (in place)
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    // trim from both ends (in place)
    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    static inline void lower(std::string & data) {
        std::transform(data.begin(), data.end(), data.begin(),
            [](unsigned char c){ return std::tolower(c); });
    }

    static constexpr unsigned int h$(const char* str, int h = 0) {
        return !str[h] ? 5381 : (h$(str, h+1) * 33) ^ str[h];
    }

    bool isBreakChar(char c) {
        return c == 27;
    };

    static constexpr const int max_search = 10;
    static constexpr const const char* version = "v1.0 Beta";

    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD CursorPosition;

    void gotoXY(int _x, int _y) {
        CursorPosition.X = _x;
        CursorPosition.Y = _y;
        SetConsoleCursorPosition(console, CursorPosition);
    };

    int printSearchedLevels(std::vector<Level*> _lvls, std::string _srch) {
        std::stringstream list;
        int found = 0;

        for (Level* lvl : _lvls) {
            std::string name = lvl->name();
            trim(name);
            trim(_srch);
            lower(name);
            lower(_srch);
            if (name.find(_srch) != std::string::npos) {
                if (found < max_search)
                    list
                        << " * " << lvl->name()
                        << " (" << lvl->length()
                        << ", " << lvl->objectCount()
                        << " objs)\n";
                found++;
            }
        }
        
        if (found < max_search)
            for (int i = 0; i < max_search - found; i++)
                list << "\n";

        else if (found == max_search)
            list << "\n";

        else list << "...\n";

        std::cout << "Found " << found << " results for \"" << _srch << "\":\n" << list.str();

        if (found < max_search)
            return found;
        else return max_search;
    };

    void processInput(int ac, char* av[]) {
        if (ac < 2) {
            std::cout << "Use \"./gdshare.exe help\" for help." << std::endl;
            return;
        }

        std::vector<std::string> args;
        for (int i = 0; i < ac; i++)
            args.assign(av + 1, av + ac);

        switch (h$(args[0].c_str())) {
            case h$("list"): {
                std::cout << "Loading levels..." << std::endl;

                CCLocalLevels* local = new CCLocalLevels([](std::string s, int p) -> void {
                    std::cout << p << "% ";
                });

                std::cout << "\n\n";

                std::vector<Level*> lvls = local->getLevels();

                if (args.size() > 1) {
                    if (args[1] == "by-name")
                        tools::sortLevelList(&lvls, tools::Sorting::Name);
                }

                for (Level* lvl : lvls)
                    std::cout << lvl->name() << std::endl;
            } break;

            case h$("find"): {
                if (args.size() < 2) {
                    std::cout << "Usage: \"find <search-term>\"" << std::endl;
                    return;
                }
                std::cout << "Loading levels..." << std::endl;

                CCLocalLevels* local = new CCLocalLevels([](std::string s, int p) -> void {
                    std::cout << p << "% ";
                });

                std::cout << "\n\n";

                std::vector<Level*> lvls = local->getLevels();

                tools::sortLevelList(&lvls, tools::Sorting::Name);

                std::string srch = args.at(1);
                int found;

                for (Level* lvl : lvls) {
                    std::string name = lvl->name();
                    trim(name);
                    trim(srch);
                    lower(name);
                    lower(srch);
                    if (name.find(srch) != std::string::npos) {
                        std::cout
                            << " * " << lvl->name()
                            << " (" << lvl->length()
                            << ", " << lvl->objectCount()
                            << " objs)\n";
                        found++;
                    }
                }

                std::cout << "\nFound " << found << " results" << std::endl;

                /*

                for (int i = 0; i < max_search + 1; i++)
                    std::cout << "\n";
                
                std::cout << "\n\n";

                std::cout <<
                    "Press ESC to exit searching.\n" <<
                    "Use arrow keys to select a found result, and press ENTER to select.\n" <<
                    "Type level name: ";

                char key;
                int typed = 0;
                int selection = 0;
                int found = 0;
                std::string searchs;
                bool ignore_next = false;

                while (!isBreakChar((key = _getch()))) {
                    //if (GetAsyncKeyState(VK_CONTROL))
                    //    continue;

                    if (ignore_next) {
                        ignore_next = false;
                        continue;
                    }

                    int clearbuf = 40;
                    bool hop = false;
                    
                    if (key == 72) {
                        if (selection < found)
                            selection++;
                        hop = true;

                        ignore_next = true;
                    }
                    if (key == 80) {
                        if (selection > 0)
                            selection--;
                        hop = true;

                        ignore_next = true;
                    }

                    if (!hop) {
                        // handle backspace
                        if (key == 8) {
                            if (typed <= 0) continue;
                            std::cout << "\b \b";
                            searchs = searchs.substr(0, searchs.size() - 1);
                            typed--;
                        } else {
                            typed++;
                            std::cout << key;
                            searchs += key;
                        }
                    }

                    CONSOLE_SCREEN_BUFFER_INFO cbsi;
                    GetConsoleScreenBufferInfo(console, &cbsi);

                    if (!hop) {
                        // clear output area
                        gotoXY(0, cbsi.dwCursorPosition.Y - max_search - 5);
                        for (int ix = 0; ix < max_search + 2; ix++)
                            std::cout << std::string(clearbuf, ' ')
                                    << std::string(clearbuf, '\b')
                                    << "\n";

                        // print found levels
                        gotoXY(0, cbsi.dwCursorPosition.Y - max_search - 5);
                        found = printSearchedLevels(lvls, searchs);
                        gotoXY(cbsi.dwCursorPosition.X, cbsi.dwCursorPosition.Y);
                    } else {
                        gotoXY(0, cbsi.dwCursorPosition.Y - 5 - selection);
                        std::cout << selection << "/" << found;
                        gotoXY(cbsi.dwCursorPosition.X, cbsi.dwCursorPosition.Y);
                    }
                }   //*/
            } break;

            case h$("export"): {
                if (args.size() < 2) {
                    std::cout << "\nUsage: \"export <level 1 name> <level 2 name> <level 3 name> ...\"\n\n"
                        << "Note: If the level name contains space(s), wrap the name in quotes \"Example name\"\n\n"
                        << "Note: You can export to a specified format with as-<format>.\n\n"
                        << "Supported formats:\n.gmd (GDShare)\n.gmd2 (GDShare (Experimental))\n.lvl (LvlShare)\n\n"
                        << "Example: \"export as-gmd SomeLevel \"Another Level\" as-lvl thirdlvl\"\n\n";
                    return;
                }

                std::cout << "Loading levels..." << std::endl;

                CCLocalLevels* local = new CCLocalLevels([](std::string s, int p) -> void {
                    std::cout << p << "% ";
                });

                std::cout << "\n\n";

                std::map<std::string, std::string> types = {
                    { "as-gmd", filetypes::GDShare },
                    { "as-gmd2", filetypes::GDShare2 },
                    { "as-lvl", filetypes::LvlShare }
                };

                std::string type = filetypes::Default;

                for (int ix = 1; ix < args.size(); ix++)
                    if (types.find(args.at(ix)) != types.end())
                        type = types.at(args.at(ix));
                    else
                        std::cout << local->exportLevel(args.at(ix), "", type).info << std::endl;
            } break;

            case h$("import"): {
                if (args.size() < 2) {
                    std::cout << "\nUsage: \"import <path/to/file> <path/to/file2> ...\"\n\n"
                        << "Note: If the file path contains space(s), wrap the path in quotes \"C:/Example Path/level.gmd\"\n\n"
                        << "Supported formats:\n.gmd (GDShare)\n.gmd2 (GDShare (Experimental))\n.lvl (LvlShare)\n\n";
                    return;
                }

                std::cout << "Loading levels..." << std::endl;

                CCLocalLevels* local = new CCLocalLevels([](std::string s, int p) -> void {
                    std::cout << p << "% ";
                });

                std::cout << "\n\n";

                for (int ix = 1; ix < args.size(); ix++)
                    std::cout << local->importLevel(args.at(ix)) << std::endl;
                
                std::cout << "Saving..." << std::endl;
                
                auto res = local->save();

                if (res.OK)
                    std::cout << "Saved!" << std::endl;
                else
                    std::cout
                        << "Error saving: " << res.info
                        << "\nNote: This means NONE of the levels"
                        << " provided have been imported into GD.\n";
            } break;

            case h$("info"): {
                if (args.size() < 2) {
                    std::cout << "\nUsage: \"info <level 1 name> <level 2 name>\"\n\n"
                        << "Note: If the level name contains space(s), wrap the name in quotes \"Example Name\"\n\n";
                    return;
                }

                std::cout << "Loading levels..." << std::endl;

                CCLocalLevels* local = new CCLocalLevels([](std::string s, int p) -> void {
                    std::cout << p << "% ";
                });

                std::cout << "\n\n";

                for (int ix = 1; ix < args.size(); ix++) {
                    Level* lvl = local->getLevel(args.at(ix));

                    if (lvl == nullptr) {
                        std::cout << "Level \"" << args.at(ix) << "\" not found!\n";
                        continue;
                    }

                    std::string etime = lvl->key(Level::Keys["editor-time"]);
                    if (etime == "") etime = "0";

                    std::cout
                        << "Name\t\t" << lvl->name() << "\n"
                        << "Creator\t\t" << lvl->key(Level::Keys["creator"]) << "\n"
                        << "Description\t" << lvl->description() << "\n"
                        << "Object count\t" << lvl->objectCount() << "\n"
                        << "Editor time\t" << std::stoi(etime) / 3600 << "h\n"
                        << "Song\t\t" << lvl->song() << "\n"
                        << "Version\t\t" << lvl->key(Level::Keys["version"]) << "\n"
                        << "Attempts\t" << lvl->key(Level::Keys["attempts"]) << "\n"
                        << "Length\t\t" << lvl->length()
                        << "\n\n";
                }
            } break;

            case h$("help"): {
                std::cout << "GDShare-CLI " << version << "\n\n"
                << "Commands:\n"
                << "export\t\tExport level(s)\n"
                << "import\t\tImport level(s)\n"
                << "list\t\tList levels\n"
                << "find\t\tFind a level\n"
                << "info\t\tView level info\n\n"
                << "For support, contact HJfod#1795 on Discord\n\n";
            } break;

            default:
                std::cout << "Unknown command \"" << args[0] << "\". " <<
                "Use \"gdshare.exe help\" to get a list of commands.\n";
        }
    }
}