#ifndef MATTERCOLLISION_H
#define MATTERCOLLISION_H

#include "VP.h"
#include "btBulletDynamicsCommon.h"


/*
    Encapsulates all data regarding a matter collision.
*/
class MatterNode;
class MatterCollision
{
    public:
        MatterCollision(MatterNode* first, MatterNode* second, const btVector3& velocityOnFirst, const btVector3& velocityOnSecond, btManifoldPoint point, double timeStep);
        virtual ~MatterCollision();

        MatterNode* getFirst() const;
        MatterNode* getSecond() const;

        const btManifoldPoint& getPoint() const;

        const btVector3& getNormalOnFirst() const;
        const btVector3& getNormalOnSecond() const;

        const btVector3& getVelocityFirst() const;
        const btVector3& getVelocitySecond() const;

    protected:
        MatterNode* mFirst;
        MatterNode* mSecond;

        btManifoldPoint mManifoldPoint;

        btVector3 mVelocityFirst;
        btVector3 mVelocitySecond;

        double mTimeStep;


    private:
};

#endif // MATTERCOLLISION_H
