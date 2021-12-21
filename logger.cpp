#include <iostream>
#include <fstream>

#define LOG_FILE "app.log"

/*
fstream interface that autocloses on write
*/
namespace std {
    class lfstream_flush {};
    const lfstream_flush FLUSH;
    
    class lfstream {
        std::ofstream stream;
        public:

        lfstream(const char* name)
        {
            stream = std::ofstream(name);
        }

        template<class T>
        lfstream& operator<<(T x)
        {
            stream << x;
            stream.flush();
            return *this;
        }

        lfstream& operator<<(lfstream_flush _)
        {
            stream.flush();
            return *this;
        }
    };
};

std::lfstream log_console(LOG_FILE);
