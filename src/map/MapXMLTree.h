//
// Created by hiram on 5/17/22.
//

#ifndef CITYY_MAPXMLTREE_H
#define CITYY_MAPXMLTREE_H
#include "rapidxml.hpp"
#include "TransitNetwork.h"
#include "rapidxml_utils.hpp"

namespace map {
    class MapXMLTree {
    public:
        enum XMLNodeType{
            NODE,
            WAY,
            RELATION,
        };

        MapXMLTree(const std::string& filePath);
        TransitNetwork generateNetwork();
    private:
        rapidxml::file<char> file;
        std::string fileContent;
        rapidxml::xml_document<char> XMLTree;
    };

} // map

#endif //CITYY_MAPXMLTREE_H
