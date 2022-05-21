//
// Created by hiram on 5/17/22.
//

#ifndef CITYY_HIGHWAY_H
#define CITYY_HIGHWAY_H

#include <optional>
#include "Way.h"
#include "HighwayType.h"

namespace map {
    typedef std::pair<int, int> Lanes;
    class Highway : public Way{
    public:
       Highway(std::string name, HighwayType type, Lanes lanes);
        Highway(HighwayType type, Lanes lanes);

        [[nodiscard]] HighwayType getType() const;

        void setType(HighwayType type);

        [[nodiscard]] const Lanes &getLanes() const;

        void setLanes(const Lanes &lanes);

        [[nodiscard]] const std::string &getName() const;

        void setName(const std::string &name);

        bool hasName();

    private:
        HighwayType type;
        std::optional<std::string> name;
        Lanes lanes;
    };
} // map

#endif //CITYY_HIGHWAY_H
