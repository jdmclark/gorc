#pragma once

#include "physicscollideclass.h"
#include "framework/flagset.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class Thing;

class PhysicsObjectData {
public:
	virtual ~PhysicsObjectData();

	unsigned int SectorId;
	FlagSet<PhysicsCollideClass> CollisionGroup, CollisionMask;
};

class SurfaceObjectData : public PhysicsObjectData {
public:
	unsigned int SurfaceId;
};

class ThingObjectData : public PhysicsObjectData {
public:
	int ThingId;
};

template <typename P, typename Q> bool MatchObjectPair(PhysicsObjectData* a, PhysicsObjectData* b, P** p_out, Q** q_out) {
	P* ap = dynamic_cast<P*>(a);
	if(ap) {
		Q* bq = dynamic_cast<Q*>(b);
		if(bq) {
			*p_out = ap;
			*q_out = bq;
			return true;
		}
	}

	Q* aq = dynamic_cast<Q*>(a);
	if(aq) {
		P* bp = dynamic_cast<P*>(b);
		if(bp) {
			*p_out = bp;
			*q_out = aq;
			return true;
		}
	}

	return false;
}

}
}
}
}
