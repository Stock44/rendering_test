//
// Created by hiram on 4/26/22.
//

#ifndef CITYY_ROAD_H
#define CITYY_ROAD_H


namespace sim {
    struct Road {
        explicit Road(int lanes = 1, float speedLimit = 80.0f, float speedModifier = 1.0f);
        float speedModifier;
        float speedLimit;
        int lanes;
    };
}

#endif //CITYY_ROAD_H
