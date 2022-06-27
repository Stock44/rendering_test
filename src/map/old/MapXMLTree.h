//
// Created by hiram on 5/17/22.
//

#ifndef CITYY_MAPXMLTREE_H
#define CITYY_MAPXMLTREE_H

#include "rapidxml.hpp"
#include "TransitNetwork.h"
#include "rapidxml_utils.hpp"

namespace map {
    static const std::unordered_map<std::string, RoadType> osmHighwayToType = {
            {"road",         UNKNOWN},
            {"motorway",     MOTORWAY},
            {"trunk",        TRUNK},
            {"primary",      PRIMARY},
            {"secondary",    SECONDARY},
            {"tertiary",     TERTIARY},
            {"unclassified", QUATERNARY},
            {"residential",  RESIDENTIAL},
            {"motorway_link",  LINK},
            {"trunk_link",  LINK},
            {"primary_link",  LINK},
            {"secondary_link",  LINK},
            {"tertiary_link",  LINK},
    };

    class MapXMLTree {
    public:
        enum class XMLNodeType {
            NODE,
            WAY,
            RELATION,
        };

        explicit MapXMLTree(const std::string &filePath);

        TransitNetwork generateNetwork();

    private:

        rapidxml::file<char> file;
        std::string fileContent;
        rapidxml::xml_document<char> XMLTree;
    };

} // map

#endif //CITYY_MAPXMLTREE_H
