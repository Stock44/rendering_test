//
// Created by hiram on 5/17/22.
//
#ifndef XML_UTILS
#define XML_UTILS
#include <rapidxml.hpp>
#include <functional>
#include <map>
#include "rapidxml_iterators.hpp"
#include <cmath>

namespace rapidxml {
    auto node_end = node_iterator<char>();
    auto attribute_end = attribute_iterator<char>();

    template <typename Ch = char>
    void processChildNodes(xml_node<Ch> *parentNode, std::function<void(xml_node<Ch>*)> forEach) {
        for(auto it = node_iterator(parentNode); it != node_end; ++it) {
            forEach(&*it);
        }
    }

    template <typename Ch = char>
    void processNodeAttributes(xml_node<Ch> *node, std::function<void(xml_attribute<Ch>*)> forEach){
        for(auto it = attribute_iterator(node); it != attribute_end; ++it) {
            forEach(&*it);
        }
    }

    template <typename Ch = char>
    std::vector<std::string_view> getWayMemberNodesIDs(xml_node<Ch> *wayNode) {
        auto nodeName = std::string_view(wayNode->name());
        if (nodeName != "way") {
            throw std::runtime_error("Error: Node is not a way!");
        }

        std::vector<std::string_view> nodeIDs;

        processChildNodes<Ch>(wayNode, [&nodeIDs] (xml_node<Ch> *childNode) {
            if (std::string_view(childNode->name()) == "nd") {
                nodeIDs.push_back(childNode->first_attribute()->value());
            }
        });

        return nodeIDs;
    }

    template <typename Ch = char>
    std::map<std::string_view, std::string_view> getTags(xml_node<Ch> *elementNode) {
        auto nodeName = std::string_view(elementNode->name());
        if (nodeName != "way" && nodeName != "node" && nodeName != "relation") {
            throw std::runtime_error("Error: Node is not an element!");
        }

        std::map<std::string_view, std::string_view> tags;
        processChildNodes<Ch>(elementNode, [&tags] (xml_node<Ch> *subelementNode) {
            if (std::string_view(subelementNode->name()) == "tag") {
                auto key = std::string_view(subelementNode->first_attribute()->value());
                auto value = std::string_view(subelementNode->first_attribute()->next_attribute()->value());
                tags[key] = value;
            }
        });
        return tags;
    }

    template <typename Ch = char>
    std::map<std::string_view, std::string_view> getAttributes(xml_node<Ch> *elementNode) {
        std::map<std::string_view, std::string_view> attributes;
        processNodeAttributes<Ch>(elementNode, [&attributes] (xml_attribute<Ch> *attribute) {
            attributes[attribute->name()] = attribute->value();
        });
        return attributes;
    }

    // first is latitude, second is longitude
    double distanceLatLon(std::pair<double, double> c1, std::pair<double,double> c2) {
        static double rad = M_PI / 180;
        double a = 0.5 - cos((c2.first - c1.first) * rad) / 2.0 +
                cos(c1.first * rad) * cos(c2.first * rad) *
                        (1 - cos((c2.second - c1.second) * rad)) / 2.0;
        return 12742000.0 * asin(sqrt(a)); // in meters
    }
}

#endif