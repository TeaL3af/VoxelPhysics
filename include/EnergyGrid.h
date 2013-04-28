#ifndef ENERGYGRID_H
#define ENERGYGRID_H

#include "Vector3.h"
#include "VoxelField.h"
#include <vector>
#include <unordered_map>
/*
    During the simulation of a collision, each Matter is represented by an energy grid.
*/

//TODO: We need to track energy separatly for each collision set.

class MatterNode;
class Matter;

class EnergyGrid
{
    public:
        EnergyGrid(MatterNode* matter);
        virtual ~EnergyGrid();

        void setEnergyPerVoxel(float energy);

        //Set the energy in each voxel
        void setInitialEnergy(float energy);
        Matter* getMatter();
        MatterNode* getMatterNode();

        Vector3f getEnergyVector();
        void setEnergy(const Vector3f& energyVector);
        void setCollisionPartner(EnergyGrid* energyGrid);

        void setAsProjector();
        void setAsReciever();

        //Transfer energy
        //Direct transfer, as in energy moves in the direction of movement and only creates pressure.
        void directTransfer();

        //Indirect transfer, as in the energised voxels attempt to bring their neighbours up to the correct level.
        //Creates stress and pressure.
        void indirectTransfer();

        //Creates a bridge from the "virtual" voxel coordinate on this shape to at point on the partner shape.
        void addBridge(Vector3i localVoxelCoord, Vector3f partnerLocalCoord);

        //Check if the matter needs to be decomposed into smaller shapes due to breaking/snapping.
        //matterArray and numMatter are returns. Returns true if new matter shapes have been created.
        bool separate(std::vector<VoxelField>& voxelFieldArray);

        //Construct Pressure maps for the grids.
        void buildMaps(const Vector3f& otherEnergy);

        void updateRenderData();

        bool validCoord(Vector3i target);

    protected:

        enum mDirection
        {
            DOWN,
            UP,
            LEFT,
            RIGHT,
            FRONT,
            BACK,
            LEFT_FRONT,
            LEFT_BACK,
            RIGHT_FRONT,
            RIGHT_BACK,
            UP_LEFT,
            UP_RIGHT,
            UP_FRONT,
            UP_BACK,
            UP_LEFT_FRONT,
            UP_LEFT_BACK,
            UP_RIGHT_FRONT,
            UP_RIGHT_BACK,
            DOWN_LEFT,
            DOWN_RIGHT,
            DOWN_FRONT,
            DOWN_BACK,
            DOWN_LEFT_FRONT,
            DOWN_LEFT_BACK,
            DOWN_RIGHT_FRONT,
            DOWN_RIGHT_BACK
        };

        static Vector3i getDirectionVector(char direction);
        static Vector3f getDirectionVectorF(char direction);
        static char getReverseDirection(char direction);

        float mRecieverMap[26]; //Each direction is given a code from 0 - 25.
                                //Depending on the direction, a different amount of stress or pressure is generated.
                                //The stored value in the map is the ratio pressure/stress.

        float mProjectorMap[26]; //Second map for when projecting energy.



        std::vector<char> mPressureDirectionsR; //All direction codes that generate pressure should be stored here.
        std::vector<char> mPressureDirectionsP;

        float* mCurrentMap; //Pointer to one of the two above maps, the one that we should currently use.
        std::vector<char>* mCurrentPressureDirections;

        float mDirectTransferMap[26]; //This map stores the portion of energy each adjanct voxel should be given

        /*
            When a voxel transfers energy to a different shape it maps onto a certain point on that shape.
            This bridge is reperesented by a virtual voxel in the home shape and a point on the other one.
        */
        std::unordered_map<int, Vector3f> mBridges;
        int getBridgeKey(const Vector3i& voxelCoord);
        Vector3i getBridgeVector(int key);
        bool getBridge(int key, Vector3f& ret_BridgePoint); //Returns true if bridge exists with it's point in rBridgePoint.

        bool mIsReciever; //False if projector

        //Represents the voxel data used in all phases of energy transfer.
        class VoxelData
        {
            public:
                VoxelData()
                {
                    mStrength = 0.0f;
                    mEnergyInProjectedDirection = 0.0f;
                    mEnergyInRecievingDirection = 0.0f;
                    mStress = 0.0f;
                    mPressure = 0.0f;
                    mFull = false;
                    mGraphed = false;
                    mDestroyed = false;
                    mSnapped = false;
                }
                float mStrength;
                float mEnergyInProjectedDirection;
                float mEnergyInRecievingDirection;
                float mPressure;
                float mStress;
                bool mGraphed; //Used in indirect transfer.

                bool mDestroyed; //Caved into pressure.
                bool mSnapped; //Snapped under stress.

                bool mFull;
                void setEnergy(bool recieving, float energy)
                {
                    if(recieving)
                    {
                        mEnergyInRecievingDirection = energy;
                    }
                    else
                    {
                        mEnergyInProjectedDirection = energy;
                    }
                }
                void addEnergy(bool recieving, float energy)
                {
                    if(recieving)
                    {
                        mEnergyInRecievingDirection += energy;
                    }
                    else
                    {
                        mEnergyInProjectedDirection += energy;
                    }
                }
                float getEnergy(bool recieving)
                {
                    if(recieving)
                    {
                        return mEnergyInRecievingDirection;
                    }
                    else
                    {
                        return mEnergyInProjectedDirection;
                    }
                }
        };

        //Apply pressure to a voxel and automatically destroy it if needed.
        void pressureVoxel(Vector3i voxelCoord, float pressure);
        //Apply stress to a voxel and automatically snap it if needed.
        void stressVoxel(Vector3i voxelCoord, float stress);



        /*
            A transfer graph is used during the "indirect transfer" phase of energy transfer.
            Every voxel non-energised voxel is given several "feeder" voxels.
            To energise a voxel, the energy is requested evenly from all feeders, which in turn request it from their feeders and so on.
            Each request generates stress and pressure along the chain.
            All requested energy is drawn from a pool, instead of trying to maintain a huge amount of consistency.
        */
        class TransferNode
        {
            public:
                bool mSource; //Source nodes have no feeders.
                int mFeeder;
                char mFeederDirection;
                Vector3i mVoxelCoord; //The voxel this node is representing.
                unsigned int mGeneration; //Which step the node was added to the graph.
                bool mDeadEnd;

                TransferNode(bool isSource, Vector3i coord)
                {
                    mSource = isSource;
                    mVoxelCoord = coord;
                    mFeeder = -1;
                    mFeederDirection = 30;
                    mDeadEnd = false;
                }
        };

        //Drag energy through the transfer graph. Return true if node is still valid.
        bool pullEnergy(unsigned int nodeIndex, float energy, char direction);

        std::vector<TransferNode> mTransferGraph;
        std::vector<std::vector<std::vector<int>>> mTransferMap;



        std::vector<std::vector<std::vector<VoxelData>>> mVoxelData;

        void buildTransferGraph();

        //Transfer energy between voxels
        void directTransferVoxel(Vector3i sourceVoxel);

        //void transferEnergy(Vector3i sourceVoxel, char direction, float energy);
        //void transferEnergy(Vector3i sourceVoxel, Vector3f direction, float energy);

        float transferEnergyFrom(Vector3i sourceVoxel, char direction);
        float transferEnergyFrom(Vector3i sourceVoxel, float energy);

        void transferEnergyTo(Vector3i targetVoxel, char direction, float energy);
        void transferEnergyTo(Vector3i targetVoxel, Vector3f direction, float energy);

        void directTransferTo(Vector3i targetVoxel, float energy);

        /* Move from a bridge backwards until the edge of the voxel field is reached, then return the total energy along that path and the point where the path ends */
        void transferInternalEnergyThroughBridge(std::unordered_map<int, Vector3f>::const_iterator bridgeIterator);

        void transferExternalEnergyTo(Vector3f pointCoord, float energy);

        bool mDestructionOccured; //Is true if there was any breakage during energy transfer.
        bool mSnappingOccured; //Is true if there was any snapping during energy transfer.

        MatterNode* mMatterNode;

        float mEnergyPerVoxel;
        float mStartingEnergy;

        Vector3f mEnergyVector;
        Vector3f mEnergyVectorLocal;

        EnergyGrid* mCollisionPartner; //The current other grid this grid is involved with.

        static const Vector3i mDirectionVectors[26];

    private:
};



#endif // ENERGYGRID_H