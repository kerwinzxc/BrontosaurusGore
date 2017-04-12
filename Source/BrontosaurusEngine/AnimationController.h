#ifndef AV_SCENEANIMATOR_H
#define AV_SCENEANIMATOR_H


#include <FbxLoader\AssImp\cimport.h>
#include <FbxLoader\assimp\scene.h>
#include <FbxLoader\assimp\postprocess.h>

#include <map>
#include <vector>
#include <fstream>
#include <tuple>
#include <string>

#include "../CommonUtilities/matrix44.h"
#include "CommonUtilities.h"

using mat4 = CU::Matrix44f;

class CBone {
public:

	std::string Name;
	mat4 Offset, LocalTransform, GlobalTransform, OriginalLocalTransform;
	CBone* Parent;
	std::vector<CBone*> Children;

	CBone() :Parent(0){}
	~CBone(){ for(size_t i(0); i< Children.size(); i++) delete Children[i]; }
};

class CAnimationChannel
{
public:
	std::string Name;
	std::vector<aiVectorKey> mPositionKeys;
	std::vector<aiQuatKey> mRotationKeys;
	std::vector<aiVectorKey> mScalingKeys;
};

class CAnimEvaluator{
public:

	CAnimEvaluator(): mLastTime(0.0f), TicksPerSecond(0.0f), Duration(0.0f), PlayAnimationForward(true) {}
	CAnimEvaluator( const aiAnimation* pAnim);
	void Evaluate( float pTime, std::map<std::string, CBone*>& bones);
	std::vector<mat4>& GetTransforms(float dt){ return Transforms[GetFrameIndexAt(dt)]; }
	std::vector<mat4>& GetTransforms(float dt, bool loop) { return Transforms[MIN(GetFrameIndexAt(dt, loop), Transforms.size() -1)]; } // Quick temp fix 

	unsigned int GetFrameIndexAt(float time);
	unsigned int GetFrameIndexAt(float time, bool loop);


	std::string Name;
	std::vector<CAnimationChannel> Channels;
	bool PlayAnimationForward;// play forward == true, play backward == false
	float mLastTime, TicksPerSecond, Duration;	
	std::vector<std::tuple<unsigned int, unsigned int, unsigned int> > mLastPositions;
	std::vector<std::vector<mat4>> Transforms;//, QuatTransforms;/** Array to return transformations results inside. */
};

namespace CU
{
	class CSerilizerSaver;
	class CSerializerLoader;
}

class CSceneAnimator
{
public:

	CSceneAnimator(): Skeleton(0), CurrentAnimIndex(-1) {}
	~CSceneAnimator(){ /*Release();*/ }

	bool CarlSave(const std::string& aFilePath);
	bool CarlSaveRecursive(CBone& aChild, CU::CSerilizerSaver& aSaver);
	bool CarlLoad(const std::string& aFilePath);
	bool CarlLoadRecursive(CBone& aChild, CU::CSerializerLoader& aLoader);

	void Init(const aiScene* pScene);// this must be called to fill the CSceneAnimator with valid data
	void Release();// frees all memory and initializes everything to a default state
	bool HasSkeleton() const { return !Bones.empty(); }// lets the caller know if there is a skeleton present
	// the set animation returns whether the animation changed or is still the same. 
	bool SetAnimIndex( int32_t pAnimIndex);// this takes an index to set the current animation to
	bool SetAnimation(const std::string& name);// this takes a string to set the animation to, i.e. SetAnimation("Idle");
	// the next two functions are good if you want to change the direction of the current animation. You could use a forward walking animation and reverse it to get a walking backwards
	void PlayAnimationForward() { Animations[CurrentAnimIndex].PlayAnimationForward = true; }
	void PlayAnimationBackward() { Animations[CurrentAnimIndex].PlayAnimationForward = false; }
	//this function will adjust the current animations speed by a percentage. So, passing 100, would do nothing, passing 50, would decrease the speed by half, and 150 increase it by 50%
	void AdjustAnimationSpeedBy(float percent) { Animations[CurrentAnimIndex].TicksPerSecond*=percent/100.0f; }
	//This will set the animation speed
	void AdjustAnimationSpeedTo(float tickspersecond) { Animations[CurrentAnimIndex].TicksPerSecond=tickspersecond; }
	// get the animationspeed... in ticks per second
	float GetAnimationSpeed() const { return Animations[CurrentAnimIndex].TicksPerSecond; }
	// get the transforms needed to pass to the vertex shader. This will wrap the dt value passed, so it is safe to pass 50000000 as a valid number
	std::vector<mat4>& GetTransforms(float dt){ return Animations[CurrentAnimIndex].GetTransforms(dt); }
	std::vector<mat4>& GetTransforms(float dt, bool loop) { return Animations[CurrentAnimIndex].GetTransforms(dt, loop); }


	int32_t GetAnimationIndex() const { return CurrentAnimIndex; }
	std::string GetAnimationName() const { return Animations[CurrentAnimIndex].Name;  }
	float GetAnimationDuration() const { return Animations[CurrentAnimIndex].Duration; }
	//GetBoneIndex will return the index of the bone given its name. The index can be used to index directly into the vector returned from GetTransform
	int GetBoneIndex(const std::string& bname){ std::map<std::string, unsigned int>::iterator found = BonesToIndex.find(bname); if(found!=BonesToIndex.end()) return found->second; else return -1;}
	//GetBoneTransform will return the matrix of the bone given its name and the time. be careful with this to make sure and send the correct dt. If the dt is different from what the model is currently at, the transform will be off
	mat4 GetBoneTransform(float dt, const std::string& bname) { int bindex=GetBoneIndex(bname); if(bindex == -1) return mat4(); return Animations[CurrentAnimIndex].GetTransforms(dt)[bindex]; }
	// same as above, except takes the index
	mat4 GetBoneTransform(float dt, unsigned int bindex) {  return Animations[CurrentAnimIndex].GetTransforms(dt)[bindex]; }

	std::vector<CAnimEvaluator> Animations;// a std::vector that holds each animation 
	int32_t CurrentAnimIndex;/** Current animation index */


	//These functions are for getting the GlobalTransform of a bone
	mat4 GetBoneGlobalTransform(const std::string& bname);
	mat4 GetBoneGlobalTransform(unsigned int bindex);

protected:

	CBone* Skeleton;/** Root node of the internal scene structure */
	std::map<std::string, CBone*> BonesByName;/** Name to node map to quickly find nodes by their name */
	std::map<std::string, unsigned int> BonesToIndex;/** Name to node map to quickly find nodes by their name */
	std::map<std::string, uint32_t> AnimationNameToId;// find animations quickly
	std::vector<CBone*> Bones;// DO NOT DELETE THESE when the destructor runs... THEY ARE JUST COPIES!!
	std::vector<mat4> Transforms;// temp array of transfrorms


	void UpdateTransforms(CBone* pNode);
	void CalculateBoneToWorldTransform(CBone* pInternalNode);/** Calculates the global transformation matrix for the given internal node */

	void Calculate( float pTime);
	void CalcBoneMatrices();

	void ExtractAnimations(const aiScene* pScene);
	CBone* CreateBoneTree( aiNode* pNode, CBone* pParent);// Recursively creates an internal node structure matching the current scene and animation. 
	
};

#endif