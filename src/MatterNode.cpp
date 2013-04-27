#include "MatterNode.h"

#include "App.h"
#include "Renderer.h"
#include "VoxelConverter.h"

MatterNode::MatterNode(App* app, GLenum renderPass, const Material* material, bool floats, const char* voxelFilename): SceneNode(app, renderPass), mMatter(app, material, floats)
{
    mMatter.import(voxelFilename);
    mMatter.setMaterial(material);
    mUpToDate = false;
}

MatterNode::MatterNode(App* app, GLenum renderPass, const Material* material, bool floats, VoxelField voxelField): SceneNode(app, renderPass), mMatter(app, material, floats)
{
    mMatter.setVoxelField(voxelField);
    mMatter.setMaterial(material);
    mUpToDate = false;
}

MatterNode::~MatterNode()
{
    //dtor
}

void MatterNode::renderSelf()
{
    mApp->getRenderer()->renderMatter(mMatter);
}

void MatterNode::simulateSelf(GLdouble deltaTime)
{
    if(!mUpToDate)
    {
        mApp->getVoxelConverter()->convert(this);
        mUpToDate = true;
    }
}

Matrix4D MatterNode::getTransform()
{
    return mMatter.getRigidBody()->getTransform();
}

Matter* MatterNode::getMatter()
{
    return &mMatter;
}

void MatterNode::setOffset(GLfloat x, GLfloat y, GLfloat z)
{
    Vector3 offset = {x, y, z};
    setOffset(offset);
}

void MatterNode::setOffset(Vector3 offset)
{
    mOffset = offset;
    mMatter.setStartingPosition(offset);
}
