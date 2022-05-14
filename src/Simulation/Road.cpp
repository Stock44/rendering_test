//
// Created by hiram on 4/26/22.
//

#include "Road.h"

sim::Road::Road(int lanes, float speedLimit, float speedModifier) : speedModifier(speedModifier), lanes(lanes),
                                                                    speedLimit(speedLimit) {}
