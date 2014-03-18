#ifndef ANAKINFLAGS_HPP
#define ANAKINFLAGS_HPP

#include <Flags.hpp>

namespace Anakin {

class AnakinFlags {
    public:
        AnakinFlags();
        Flags* getFlags(bool verbose=true);
    protected:
    private:
};
};

#endif // ANAKINFLAGS_HPP
