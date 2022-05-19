//
// Created by hiram on 5/17/22.
//

#ifndef CITYY_HIGHWAY_H
#define CITYY_HIGHWAY_H

#include "Way.h"
#include "HighwayType.h"

namespace map {
    class Highway : public Way{
    public:
       Highway (HighwayType type);

        HighwayType getType() const;

        void setType(HighwayType type);

    private:
        HighwayType type;
    };
} // map

#endif //CITYY_HIGHWAY_H
