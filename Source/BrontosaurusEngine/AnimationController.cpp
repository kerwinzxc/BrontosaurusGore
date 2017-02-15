#include "stdafx.h"

#include "AnimationController.h"

void TransformMatrix(mat4& out,const aiMatrix4x4& in){// there is some type of alignment issue with my mat4 and the aimatrix4x4 class, so the copy must be manually
	out.m11=in.a1;
	out.m12=in.a2;
	out.m13=in.a3;
	out.m14=in.a4;

	out.m21=in.b1;
	out.m22=in.b2;
	out.m23=in.b3;
	out.m24=in.b4;

	out.m31=in.c1;
	out.m32=in.c2;
	out.m33=in.c3;
	out.m34=in.c4;

	out.m41=in.d1;
	out.m42=in.d2;
	out.m43=in.d3;
	out.m44=in.d4;
}
// ------------------------------------------------------------------------------------------------
// Constructor on a given animation. 
CAnimEvaluator::CAnimEvaluator( const aiAnimation* pAnim) {
	PlayAnimationForward = true;
	mLastTime = 0.0;
	TicksPerSecond = static_cast<float>(pAnim->mTicksPerSecond != 0.0f ? pAnim->mTicksPerSecond : 100.0f);
	Duration = static_cast<float>(pAnim->mDuration);
	Name = pAnim->mName.data;
	//OUTPUT_DEBUG_MSG("Creating Animation named: "<<Name);
	Channels.resize(pAnim->mNumChannels);
	for( unsigned int a = 0; a < pAnim->mNumChannels; a++){		
		Channels[a].Name = pAnim->mChannels[a]->mNodeName.data;
		for(unsigned int i(0); i< pAnim->mChannels[a]->mNumPositionKeys; i++) Channels[a].mPositionKeys.push_back(pAnim->mChannels[a]->mPositionKeys[i]);
		for(unsigned int i(0); i< pAnim->mChannels[a]->mNumRotationKeys; i++) Channels[a].mRotationKeys.push_back(pAnim->mChannels[a]->mRotationKeys[i]);
		for(unsigned int i(0); i< pAnim->mChannels[a]->mNumScalingKeys; i++) Channels[a].mScalingKeys.push_back(pAnim->mChannels[a]->mScalingKeys[i]);
	}
	mLastPositions.resize( pAnim->mNumChannels, std::make_tuple( 0, 0, 0));
	//OUTPUT_DEBUG_MSG("Finished Creating Animation named: "<<Name);
}

unsigned int CAnimEvaluator::GetFrameIndexAt(float ptime){
	// get a [0.f ... 1.f) value by allowing the percent to wrap around 1
	ptime *= TicksPerSecond;
	
	float time = 0.0f;
	if( Duration > 0.0)
		time = fmod( ptime, Duration);

	float percent = time / Duration;
	if(!PlayAnimationForward) percent= (percent-1.0f)*-1.0f;// this will invert the percent so the animation plays backwards
	return static_cast<unsigned int>(( static_cast<float>(Transforms.size()) * percent));
}

unsigned int CAnimEvaluator::GetFrameIndexAt(float ptime, bool loop)
{
	// get a [0.f ... 1.f) value by allowing the percent to wrap around 1
	ptime *= TicksPerSecond;

	float time = 0.0f;

	if (loop == true)
	{
		if (Duration > 0.0)
			time = fmod(ptime, Duration);
	}
	else
	{
		time = ptime;
	}


	float percent = time / Duration;
	if (loop == false)
	{
		if (percent > 0.90f)
		{				
			percent = 0.90f;
		}
	}
	if (!PlayAnimationForward) percent = (percent - 1.0f)*-1.0f;// this will invert the percent so the animation plays backwards
	


	unsigned int chosenIndex = static_cast<unsigned int>((static_cast<float>(Transforms.size()) * percent));
	return (chosenIndex < Transforms.size()) ? chosenIndex : Transforms.size() - 1;
}

// ------------------------------------------------------------------------------------------------
// Evaluates the animation tracks for a given time stamp. 
void CAnimEvaluator::Evaluate( float pTime, std::map<std::string, CBone*>& bones) 
{

	pTime *= TicksPerSecond;
	
	float time = 0.0f;
	if( Duration > 0.0)
		time = fmod( pTime, Duration);

	// calculate the transformations for each animation channel
	for( unsigned int a = 0; a < Channels.size(); a++){
		const CAnimationChannel* channel = &Channels[a];
		std::map<std::string, CBone*>::iterator bonenode = bones.find(channel->Name);

		if(bonenode == bones.end()) { 
			//OUTPUT_DEBUG_MSG("did not find the bone node "<<channel->Name);
			continue;
		}

		// ******** Position *****
		aiVector3D presentPosition( 0, 0, 0);
		if( channel->mPositionKeys.size() > 0){
			// Look for present frame number. Search from last position if time is after the last time, else from beginning
			// Should be much quicker than always looking from start for the average use case.
			unsigned int frame = (time >= mLastTime) ? std::get<0>(mLastPositions[a]): 0;
			while( frame < channel->mPositionKeys.size() - 1){
				if( time < channel->mPositionKeys[frame+1].mTime){
					break;
				}
				frame++;
			}

			// interpolate between this frame's value and next frame's value
			unsigned int nextFrame = (frame + 1) % channel->mPositionKeys.size();
	
			const aiVectorKey& key = channel->mPositionKeys[frame];
			const aiVectorKey& nextKey = channel->mPositionKeys[nextFrame];
			double diffTime = nextKey.mTime - key.mTime;
			if( diffTime < 0.0)
				diffTime += Duration;
			if( diffTime > 0) {
				float factor = float( (time - key.mTime) / diffTime);
				presentPosition = key.mValue + (nextKey.mValue - key.mValue) * factor;
			} else {
				presentPosition = key.mValue;
			}
			std::get<0>(mLastPositions[a]) = frame;
		}
		// ******** Rotation *********
		aiQuaternion presentRotation( 1, 0, 0, 0);
		if( channel->mRotationKeys.size() > 0)
		{
			unsigned int frame = (time >= mLastTime) ? std::get<1>(mLastPositions[a]) : 0;
			while( frame < channel->mRotationKeys.size()  - 1){
				if( time < channel->mRotationKeys[frame+1].mTime)
					break;
				frame++;
			}

			// interpolate between this frame's value and next frame's value
			unsigned int nextFrame = (frame + 1) % channel->mRotationKeys.size() ;

			const aiQuatKey& key = channel->mRotationKeys[frame];
			const aiQuatKey& nextKey = channel->mRotationKeys[nextFrame];
			double diffTime = nextKey.mTime - key.mTime;
			if( diffTime < 0.0) diffTime += Duration;
			if( diffTime > 0) {
				float factor = float( (time - key.mTime) / diffTime);
				aiQuaternion::Interpolate( presentRotation, key.mValue, nextKey.mValue, factor);
			} else presentRotation = key.mValue;
			std::get<1>(mLastPositions[a]) = frame;
		}

		// ******** Scaling **********
		aiVector3D presentScaling( 1, 1, 1);
		if( channel->mScalingKeys.size() > 0) {
			unsigned int frame = (time >= mLastTime) ? std::get<2>(mLastPositions[a]) : 0;
			while( frame < channel->mScalingKeys.size() - 1){
				if( time < channel->mScalingKeys[frame+1].mTime)
					break;
				frame++;
			}

			presentScaling = channel->mScalingKeys[frame].mValue;
			std::get<2>(mLastPositions[a]) = frame;
		}

		aiMatrix4x4 mat = aiMatrix4x4( presentRotation.GetMatrix());

		mat.a1 *= presentScaling.x; mat.b1 *= presentScaling.x; mat.c1 *= presentScaling.x;
		mat.a2 *= presentScaling.y; mat.b2 *= presentScaling.y; mat.c2 *= presentScaling.y;
		mat.a3 *= presentScaling.z; mat.b3 *= presentScaling.z; mat.c3 *= presentScaling.z;
		mat.a4 = presentPosition.x; mat.b4 = presentPosition.y; mat.c4 = presentPosition.z;
		mat.Transpose();
		
		TransformMatrix(bonenode->second->LocalTransform, mat);
	}
	mLastTime = time;
}

void CSceneAnimator::Release(){// this should clean everything up 
	CurrentAnimIndex = -1;
	Animations.clear();// clear all animations
	delete Skeleton;// This node will delete all children recursivly
	Skeleton = NULL;// make sure to zero it out
}

// this will build the skeleton based on the scene passed to it and CLEAR EVERYTHING
void CSceneAnimator::Init(const aiScene* pScene)
{
	if(!pScene->HasAnimations()) return;
	Release();
	
	Skeleton = CreateBoneTree( pScene->mRootNode, NULL);
	ExtractAnimations(pScene);
	
	for (unsigned int i = 0; i < pScene->mNumMeshes;++i)
	{
		const aiMesh* mesh = pScene->mMeshes[i];
		
		for (unsigned int n = 0; n < mesh->mNumBones;++n)
		{
			const aiBone* bone = mesh->mBones[n];
			std::map<std::string, CBone*>::iterator found = BonesByName.find(bone->mName.data);
			if(found != BonesByName.end())
			{// FOUND IT!!! woohoo, make sure its not already in the bone list
				bool skip = false;
				for(size_t j(0); j< Bones.size(); j++)
				{
					std::string bname = bone->mName.data;
					if(Bones[j]->Name == bname) {
						skip = true;// already inserted, skip this so as not to insert the same bone multiple times
						break;
					}
				}
				if(!skip)
				{// only insert the bone if it has not already been inserted
					std::string tes = found->second->Name;
					TransformMatrix(found->second->Offset, bone->mOffsetMatrix);
					found->second->Offset.Transpose();// transpoce their matrix to get in the correct format
					Bones.push_back(found->second);
					BonesToIndex[found->first] = (unsigned int)Bones.size()-1;
				}
			} 
		}
	}
	Transforms.resize( Bones.size());
	float timestep = 1.0f/30.0f;// 30 per second
	for(size_t i(0); i< Animations.size(); i++)
	{// pre calculate the animations
		SetAnimIndex((int32_t)i);
		float dt = 0;
		for(float ticks = 0; ticks < Animations[i].Duration; ticks += Animations[i].TicksPerSecond/30.0f)
		{
			dt +=timestep;
			Calculate(dt);
			Animations[i].Transforms.push_back(std::vector<mat4>());
			std::vector<mat4>& trans = Animations[i].Transforms.back();
			for( size_t a = 0; a < Transforms.size(); ++a)
			{
				mat4 rotationmat =  Bones[a]->Offset * Bones[a]->GlobalTransform;
				trans.push_back(rotationmat);
			}
		}
	}
//	OUTPUT_DEBUG_MSG("Finished loading animations with "<<Bones.size()<<" bones");
}

void CSceneAnimator::ExtractAnimations(const aiScene* pScene)
{
	//OUTPUT_DEBUG_MSG("Extracting Animations . . ");
	for(size_t i(0); i< pScene->mNumAnimations; i++){
		Animations.push_back(CAnimEvaluator(pScene->mAnimations[i]) );// add the animations
	}
	for(uint32_t i(0); i< Animations.size(); i++){// get all the animation names so I can reference them by name and get the correct id
		AnimationNameToId.insert(std::map<std::string, uint32_t>::value_type(Animations[i].Name, i));
	}
	CurrentAnimIndex=0;
	SetAnimation("Idle");
}
bool CSceneAnimator::SetAnimation(const std::string& name)
{
	std::map<std::string, uint32_t>::iterator itr = AnimationNameToId.find(name);
	int32_t oldindex = CurrentAnimIndex;
	if(itr !=AnimationNameToId.end()) CurrentAnimIndex = itr->second;
	return oldindex != CurrentAnimIndex;
}
bool CSceneAnimator::SetAnimIndex( int32_t  pAnimIndex)
{
	if(pAnimIndex >= static_cast<int32_t>(Animations.size())) return false;// no change, or the animations data is out of bounds
	int32_t oldindex = CurrentAnimIndex;
	CurrentAnimIndex = pAnimIndex;// only set this after the checks for good data and the object was actually inserted
	return oldindex != CurrentAnimIndex;
}

// ------------------------------------------------------------------------------------------------
// Calculates the node transformations for the scene. 
void CSceneAnimator::Calculate(float pTime)
{
	if( (CurrentAnimIndex < 0) | (CurrentAnimIndex >= static_cast<int32_t>(Animations.size())) ) return;// invalid animation
	
	Animations[CurrentAnimIndex].Evaluate( pTime, BonesByName);
	UpdateTransforms(Skeleton);
}

//void UQTtoUDQ(vec4* dual, quat q, vec4& tran ) 
//{
//    dual[0].x = q.x ; 
//    dual[0].y = q.y ; 
//    dual[0].z = q.z ; 
//    dual[0].w = q.w ; 
//    dual[1].x = 0.5f *  ( tran[0] * q.w + tran[1] * q.z - tran[2] * q.y ) ; 
//    dual[1].y = 0.5f *  (-tran[0] * q.z + tran[1] * q.w + tran[2] * q.x ) ; 
//    dual[1].z = 0.5f *  ( tran[0] * q.y - tran[1] * q.x + tran[2] * q.w ) ; 
//    dual[1].w = -0.5f * ( tran[0] * q.x + tran[1] * q.y + tran[2] * q.z ) ; 
//}

// ------------------------------------------------------------------------------------------------
// Calculates the bone matrices for the given mesh. 
void CSceneAnimator::CalcBoneMatrices()
{
	for( size_t a = 0; a < Transforms.size(); ++a)
	{
		Transforms[a] =  Bones[a]->Offset * Bones[a]->GlobalTransform;
	}
}

// ------------------------------------------------------------------------------------------------
// Recursively creates an internal node structure matching the current scene and animation.
CBone* CSceneAnimator::CreateBoneTree( aiNode* pNode, CBone* pParent)
{
	CBone* internalNode = new CBone();// create a node
	internalNode->Name = pNode->mName.data;// get the name of the bone
	internalNode->Parent = pParent; //set the parent, in the case this is theroot node, it will be null

	BonesByName[internalNode->Name] = internalNode;// use the name as a key
	TransformMatrix(internalNode->LocalTransform, pNode->mTransformation);
	internalNode->LocalTransform.Transpose();
	internalNode->OriginalLocalTransform = internalNode->LocalTransform;// a copy saved
	CalculateBoneToWorldTransform(internalNode);

	// continue for all child nodes and assign the created internal nodes as our children
	for( unsigned int a = 0; a < pNode->mNumChildren; a++)
	{// recursivly call this function on all children
		internalNode->Children.push_back(CreateBoneTree( pNode->mChildren[a], internalNode));
	}
	return internalNode;
}

mat4 CSceneAnimator::GetBoneGlobalTransform(const std::string& bname)
{
	int bindex = GetBoneIndex(bname); 
	if (bindex == -1) return mat4(); 


	return GetBoneGlobalTransform(bindex);
}

mat4 CSceneAnimator::GetBoneGlobalTransform(unsigned int bindex)
{
	return Bones[bindex]->GlobalTransform;
}

// ------------------------------------------------------------------------------------------------
// Recursively updates the internal node transformations from the given matrix array
void CSceneAnimator::UpdateTransforms(CBone* pNode) 
{
	CalculateBoneToWorldTransform( pNode);// update global transform as well
	for( std::vector<CBone*>::iterator it = pNode->Children.begin(); it != pNode->Children.end(); ++it)// continue for all children
		UpdateTransforms( *it);
}

// ------------------------------------------------------------------------------------------------
// Calculates the global transformation matrix for the given internal node
void CSceneAnimator::CalculateBoneToWorldTransform(CBone* child)
{
	child->GlobalTransform = child->LocalTransform;
	CBone* parent = child->Parent;
	while( parent )
	{// this will climb the nodes up along through the parents concentating all the matrices to get the Object to World transform, or in this case, the Bone To World transform
		child->GlobalTransform *= parent->LocalTransform;
		parent  = parent->Parent;// get the parent of the bone we are working on 
	}
}
