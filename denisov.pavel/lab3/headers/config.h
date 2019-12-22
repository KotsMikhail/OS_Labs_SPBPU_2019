#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <vector>
#include <map>

class CONFIG
{
public:
    enum class KEYS
    {
        WRITERS_NUM_THREADS = 0,
        WRITERS_NUM_RECORDS,
        READERS_NUM_THREADS,
        READERS_NUM_RECORDS,
        GENERAL_N,
        GENERAL_WRITERS,
        GENERAL_READERS,
        NUM_ITERATIONS
    };

    static CONFIG * GetInstance (const char *path = nullptr);
    static void Clear (void);

    bool Load     (void);
    int  GetValue (KEYS key) const;

private:
    CONFIG () = delete;
    CONFIG (const CONFIG&) = delete;
    CONFIG & operator= (const CONFIG&) = delete;

    CONFIG (std::string &path);

    static CONFIG * instance;
    static std::vector<std::string> keysNames;

    std::string filePath;
    std::map<std::string, int> values;
};

#endif //__CONFIG_H__
