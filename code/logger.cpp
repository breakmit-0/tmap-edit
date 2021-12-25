#include <iostream>
#include <fstream>

/*
fstream interface that autocloses on write
*/
namespace std {
    class lfstream_flush {};
    const lfstream_flush FLUSH;
    
    class lfstream {
    public:
        std::ofstream stream;

        lfstream() {
            stream = std::ofstream();
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

std::lfstream log_console;
