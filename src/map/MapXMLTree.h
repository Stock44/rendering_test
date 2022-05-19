//
// Created by hiram on 5/17/22.
//

#ifndef CITYY_MAPXMLTREE_H
#define CITYY_MAPXMLTREE_H

#include "rapidxml.hpp"
#include "TransitNetwork.h"
#include "rapidxml_utils.hpp"

namespace map {
    static std::unordered_map<std::string, HighwayType> osmHighwayToType = {
            {"road",         UNKNOWN},
            {"motorway",     MOTORWAY},
            {"trunk",        TRUNK},
            {"primary",      PRIMARY},
            {"secondary",    SECONDARY},
            {"tertiary",     TERTIARY},
            {"unclassified", QUATERNARY},
            {"residential",  RESIDENTIAL},
    };

    class MapXMLTree {
    public:
        enum XMLNodeType {
            NODE,
            WAY,
            RELATION,
        };

        MapXMLTree(const std::string &filePath);

        TransitNetwork generateNetwork();

    private:

        rapidxml::file<char> file;
        std::string fileContent;
        rapidxml::xml_document<char> XMLTree;
    };

} // map

#endif //CITYY_MAPXMLTREE_H
