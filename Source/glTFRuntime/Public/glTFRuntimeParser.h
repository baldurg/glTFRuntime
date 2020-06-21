// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonValue.h"
#include "Engine/StaticMesh.h"
#include "glTFRuntimeAnimationCurve.h"

#include "glTFRuntimeParser.generated.h"

USTRUCT(BlueprintType)
struct FglTFRuntimeScene
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
	int32 Index;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
	TArray<int32> RootNodesIndices;
};


USTRUCT(BlueprintType)
struct FglTFRuntimeNode
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
	int32 Index;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
	FTransform Transform;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
	int32 MeshIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
	int32 SkinIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
	TArray<int32> ChildrenIndices;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
	int32 ParentIndex;

	FglTFRuntimeNode()
	{
		Index = INDEX_NONE;
		Transform = FTransform::Identity;
		ParentIndex = INDEX_NONE;
		MeshIndex = INDEX_NONE;
		SkinIndex = INDEX_NONE;
	}
};

UENUM()
enum class EglTFRuntimeMaterialType : uint8
{
	Opaque,
	Translucent,
	TwoSided,
	TwoSidedTranslucent,
};

UENUM()
enum class EglTFRuntimeCacheMode : uint8
{
	ReadWrite,
	None,
	Read,
	Write
};

USTRUCT(BlueprintType)
struct FglTFRuntimeSocket
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Transform;
};

USTRUCT(BlueprintType)
struct FglTFRuntimeMaterialsConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, UMaterialInterface*> MaterialsOverrideMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, UTexture2D*> TexturesOverrideMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, UTexture2D*> ImagesOverrideMap;
};

USTRUCT(BlueprintType)
struct FglTFRuntimeStaticMeshConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EglTFRuntimeCacheMode CacheMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReverseWinding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bBuildSimpleCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FBox> BoxCollisions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector4> SphereCollisions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* ComplexCollisionMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FglTFRuntimeMaterialsConfig MaterialsConfig;
};

USTRUCT(BlueprintType)
struct FglTFRuntimeSkeletalMeshConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EglTFRuntimeCacheMode CacheMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RootNodeIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FTransform> CustomSkeleton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FglTFRuntimeSocket> Sockets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FglTFRuntimeMaterialsConfig MaterialsConfig;

	FglTFRuntimeSkeletalMeshConfig()
	{
		RootNodeIndex = INDEX_NONE;
	}
};

USTRUCT(BlueprintType)
struct FglTFRuntimeSkeletalAnimationConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EglTFRuntimeCacheMode CacheMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RootNodeIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRootMotion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRemoveRootMotion;

	FglTFRuntimeSkeletalAnimationConfig()
	{
		RootNodeIndex = INDEX_NONE;
		bRootMotion = false;
	}
};

struct FglTFRuntimeUInt16Vector4
{
	uint16 X;
	uint16 Y;
	uint16 Z;
	uint16 W;

	FglTFRuntimeUInt16Vector4()
	{
		X = 0;
		Y = 0;
		Z = 0;
		W = 0;
	}

	uint16& operator[](int32 Index)
	{
		check(Index >= 0 && Index < 4);
		switch (Index)
		{
		case 0:
			return X;
		case 1:
			return Y;
		case 2:
			return Z;
		case 3:
		default:
			return W;
		}
		;
	}
};

struct FglTFRuntimePrimitive
{
	TArray<FVector> Positions;
	TArray<FVector> Normals;
	TArray<FVector4> Tangents;
	TArray<TArray<FVector2D>> UVs;
	TArray<uint32> Indices;
	UMaterialInterface* Material;
	TArray<TArray<FglTFRuntimeUInt16Vector4>> Joints;
	TArray<TArray<FVector4>> Weights;
};

/**
 *
 */
class GLTFRUNTIME_API FglTFRuntimeParser : public FGCObject
{
public:
	FglTFRuntimeParser(TSharedRef<FJsonObject> JsonObject, FMatrix InSceneBasis, float InSceneScale);
	FglTFRuntimeParser(TSharedRef<FJsonObject> JsonObject);
	static TSharedPtr<FglTFRuntimeParser> FromFilename(FString Filename);

	UStaticMesh* LoadStaticMesh(const int32 MeshIndex, const FglTFRuntimeStaticMeshConfig& StaticMeshConfig);
	bool LoadStaticMeshes(TArray<UStaticMesh*>& StaticMeshes, const FglTFRuntimeStaticMeshConfig& StaticMeshConfig);

	UStaticMesh* LoadStaticMeshByName(const FString MeshName, const FglTFRuntimeStaticMeshConfig& StaticMeshConfig);

	UMaterialInterface* LoadMaterial(const int32 MaterialIndex, const FglTFRuntimeMaterialsConfig& MaterialsConfig);
	UTexture2D* LoadTexture(const int32 TextureIndex, const FglTFRuntimeMaterialsConfig& MaterialsConfig);

	bool LoadNodes();
	bool LoadNode(int32 NodeIndex, FglTFRuntimeNode& Node);
	bool LoadNodeByName(FString NodeName, FglTFRuntimeNode& Node);

	bool LoadScenes(TArray<FglTFRuntimeScene>& Scenes);
	bool LoadScene(int32 SceneIndex, FglTFRuntimeScene& Scene);

	USkeletalMesh* LoadSkeletalMesh(const int32 MeshIndex, const int32 SkinIndex, const FglTFRuntimeSkeletalMeshConfig& SkeletalMeshConfig);
	UAnimSequence* LoadSkeletalAnimation(USkeletalMesh* SkeletalMesh, const int32 AnimationIndex, const FglTFRuntimeSkeletalAnimationConfig& AnimationConfig);
	UAnimSequence* LoadSkeletalAnimationByName(USkeletalMesh* SkeletalMesh, const FString AnimationName, const FglTFRuntimeSkeletalAnimationConfig& AnimationConfig);

	UglTFRuntimeAnimationCurve* LoadNodeAnimationCurve(const int32 NodeIndex);

	bool GetBuffer(int32 BufferIndex, TArray<uint8>& Bytes);
	bool GetBufferView(int32 BufferViewIndex, TArray<uint8>& Bytes, int64& Stride);
	bool GetAccessor(int32 AccessorIndex, int64& ComponentType, int64& Stride, int64& Elements, int64& ElementSize, int64& Count, TArray<uint8>& Bytes);

	bool GetAllNodes(TArray<FglTFRuntimeNode>& Nodes);

	int64 GetComponentTypeSize(const int64 ComponentType) const;
	int64 GetTypeSize(const FString Type) const;

	bool ParseBase64Uri(const FString Uri, TArray<uint8>& Bytes);

	void AddReferencedObjects(FReferenceCollector& Collector);

	bool LoadPrimitives(const TArray<TSharedPtr<FJsonValue>>* JsonPrimitives, TArray<FglTFRuntimePrimitive>& Primitives, const FglTFRuntimeMaterialsConfig& MaterialsConfig);
	bool LoadPrimitive(TSharedRef<FJsonObject> JsonPrimitiveObject, FglTFRuntimePrimitive& Primitive, const FglTFRuntimeMaterialsConfig& MaterialsConfig);

	void ClearErrors();

protected:
	TSharedRef<FJsonObject> Root;

	TMap<int32, UStaticMesh*> StaticMeshesCache;
	TMap<int32, UMaterialInterface*> MaterialsCache;
	TMap<int32, USkeleton*> SkeletonsCache;
	TMap<int32, USkeletalMesh*> SkeletalMeshesCache;
	TMap<int32, UTexture2D*> TexturesCache;

	TMap<int32, TArray<uint8>> BuffersCache;

	TArray<FglTFRuntimeNode> AllNodesCache;
	bool bAllNodesCached;

	UStaticMesh* LoadStaticMesh_Internal(TSharedRef<FJsonObject> JsonMeshObject, const FglTFRuntimeStaticMeshConfig& StaticMeshConfig);
	UMaterialInterface* LoadMaterial_Internal(TSharedRef<FJsonObject> JsonMaterialObject, const FglTFRuntimeMaterialsConfig& MaterialsConfig);
	bool LoadNode_Internal(int32 Index, TSharedRef<FJsonObject> JsonNodeObject, int32 NodesCount, FglTFRuntimeNode& Node);

	USkeletalMesh* LoadSkeletalMesh_Internal(TSharedRef<FJsonObject> JsonMeshObject, TSharedRef<FJsonObject> JsonSkinObject, const FglTFRuntimeSkeletalMeshConfig& SkeletalMeshConfig);
	bool LoadSkeletalAnimation_Internal(TSharedRef<FJsonObject> JsonAnimationObject, TMap<FString, FRawAnimSequenceTrack>& Tracks, float& Duration, int32& NumFrames);

	bool LoadAnimation_Internal(TSharedRef<FJsonObject> JsonAnimationObject, float& Duration, int32& NumFrames, TFunctionRef<void(const FglTFRuntimeNode& Node, const FString& Path, const TArray<float> Timeline, const TArray<FVector4> Values)> Callback, TFunctionRef<bool(const FglTFRuntimeNode& Node)> NodeFilter);

	bool FillReferenceSkeleton(TSharedRef<FJsonObject> JsonSkinObject, FReferenceSkeleton& RefSkeleton, TMap<int32, FName>& BoneMap, const FglTFRuntimeSkeletalMeshConfig& SkeletalMeshConfig);
	bool TraverseJoints(FReferenceSkeletonModifier& Modifier, int32 Parent, FglTFRuntimeNode& Node, const TArray<int32>& Joints, TMap<int32, FName>& BoneMap, const TMap<int32, FMatrix>& InverseBindMatricesMap);

	void FixNodeParent(FglTFRuntimeNode& Node);

	int32 FindCommonRoot(TArray<int32> NodeIndices);
	int32 FindTopRoot(int32 NodeIndex);
	bool HasRoot(int32 NodeIndex, int32 RootIndex);

	bool CheckJsonIndex(TSharedRef<FJsonObject> JsonObject, const FString FieldName, const int32 Index, TArray<TSharedRef<FJsonValue>>& JsonItems);
	bool CheckJsonRootIndex(const FString FieldName, const int32 Index, TArray<TSharedRef<FJsonValue>>& JsonItems) { return CheckJsonIndex(Root, FieldName, Index, JsonItems); }
	TSharedPtr<FJsonObject> GetJsonObjectFromIndex(TSharedRef<FJsonObject> JsonObject, const FString FieldName, const int32 Index);
	TSharedPtr<FJsonObject> GetJsonObjectFromRootIndex(const FString FieldName, const int32 Index) { return GetJsonObjectFromIndex(Root, FieldName, Index); }

	FString GetJsonObjectString(TSharedRef<FJsonObject> JsonObject, const FString FieldName, const FString DefaultValue);
	int32 GetJsonObjectIndex(TSharedRef<FJsonObject> JsonObject, const FString FieldName, const int32 DefaultValue);

	bool FillJsonMatrix(const TArray<TSharedPtr<FJsonValue>>* JsonMatrixValues, FMatrix& Matrix);

	float FindBestFrames(TArray<float> FramesTimes, float WantedTime, int32& FirstIndex, int32& SecondIndex);

	void NormalizeSkeletonScale(FReferenceSkeleton& RefSkeleton);
	void NormalizeSkeletonBoneScale(FReferenceSkeletonModifier& Modifier, const int32 BoneIndex, FVector BoneScale);

	bool AssignTexCoord(TSharedPtr<FJsonObject> JsonTextureObject, UMaterialInstanceDynamic* Material, const FName MaterialParam);

	bool CanReadFromCache(const EglTFRuntimeCacheMode CacheMode) { return CacheMode == EglTFRuntimeCacheMode::Read || CacheMode == EglTFRuntimeCacheMode::ReadWrite; }
	bool CanWriteToCache(const EglTFRuntimeCacheMode CacheMode) { return CacheMode == EglTFRuntimeCacheMode::Write || CacheMode == EglTFRuntimeCacheMode::ReadWrite; }

	void AddError(const FString ErrorContext, const FString ErrorMessage);

	FMatrix SceneBasis;
	float SceneScale;

	TMap<EglTFRuntimeMaterialType, UMaterialInterface*> MaterialsMap;

	TArray<FString> Errors;

	template<typename T, typename Callback>
	bool BuildFromAccessorField(TSharedRef<FJsonObject> JsonObject, const FString Name, TArray<T>& Data, const TArray<int64> SupportedElements, const TArray<int64> SupportedTypes, const bool bNormalized, Callback Filter)
	{
		int64 AccessorIndex;
		if (!JsonObject->TryGetNumberField(Name, AccessorIndex))
			return false;

		TArray<uint8> Bytes;
		int64 ComponentType, Stride, Elements, ElementSize, Count;
		if (!GetAccessor(AccessorIndex, ComponentType, Stride, Elements, ElementSize, Count, Bytes))
			return false;

		if (!SupportedElements.Contains(Elements))
			return false;

		if (!SupportedTypes.Contains(ComponentType))
			return false;

		for (int64 ElementIndex = 0; ElementIndex < Count; ElementIndex++)
		{
			int64 Index = ElementIndex * Stride;
			T Value;
			// FLOAT
			if (ComponentType == 5126)
			{
				float* Ptr = (float*)&(Bytes[Index]);
				for (int32 i = 0; i < Elements; i++)
				{
					Value[i] = Ptr[i];
				}
			}
			// BYTE
			else if (ComponentType == 5120)
			{
				int8* Ptr = (int8*)&(Bytes[Index]);
				for (int32 i = 0; i < Elements; i++)
				{
					Value[i] = bNormalized ? FMath::Max(((float)Ptr[i]) / 127.f, -1.f) : Ptr[i];
				}

			}
			// UNSIGNED_BYTE
			else if (ComponentType == 5121)
			{
				uint8* Ptr = (uint8*)&(Bytes[Index]);
				for (int32 i = 0; i < Elements; i++)
				{
					Value[i] = bNormalized ? ((float)Ptr[i]) / 255.f : Ptr[i];
				}
			}
			// SHORT
			else if (ComponentType == 5122)
			{
				int16* Ptr = (int16*)&(Bytes[Index]);
				for (int32 i = 0; i < Elements; i++)
				{
					Value[i] = bNormalized ? FMath::Max(((float)Ptr[i]) / 32767.f, -1.f) : Ptr[i];
				}
			}
			// UNSIGNED_SHORT
			else if (ComponentType == 5123)
			{
				uint16* Ptr = (uint16*)&(Bytes[Index]);
				for (int32 i = 0; i < Elements; i++)
				{
					Value[i] = bNormalized ? ((float)Ptr[i]) / 65535.f : Ptr[i];
				}
			}
			else
			{
				return false;
			}

			Data.Add(Filter(Value));
		}

		return true;
	}

	template<typename T, typename Callback>
	bool BuildFromAccessorField(TSharedRef<FJsonObject> JsonObject, const FString Name, TArray<T>& Data, const TArray<int64> SupportedTypes, const bool bNormalized, Callback Filter)
	{
		int64 AccessorIndex;
		if (!JsonObject->TryGetNumberField(Name, AccessorIndex))
			return false;

		TArray<uint8> Bytes;
		int64 ComponentType, Stride, Elements, ElementSize, Count;
		if (!GetAccessor(AccessorIndex, ComponentType, Stride, Elements, ElementSize, Count, Bytes))
			return false;

		if (Elements != 1)
			return false;

		if (!SupportedTypes.Contains(ComponentType))
			return false;

		for (int64 ElementIndex = 0; ElementIndex < Count; ElementIndex++)
		{
			int64 Index = ElementIndex * Stride;
			T Value;
			// FLOAT
			if (ComponentType == 5126)
			{
				float* Ptr = (float*)&(Bytes[Index]);
				Value = *Ptr;
			}
			// BYTE
			else if (ComponentType == 5120)
			{
				int8* Ptr = (int8*)&(Bytes[Index]);
				Value = bNormalized ? FMath::Max(((float)(*Ptr)) / 127.f, -1.f) : *Ptr;

			}
			// UNSIGNED_BYTE
			else if (ComponentType == 5121)
			{
				uint8* Ptr = (uint8*)&(Bytes[Index]);
				Value = bNormalized ? ((float)(*Ptr)) / 255.f : *Ptr;

			}
			// SHORT
			else if (ComponentType == 5122)
			{
				int16* Ptr = (int16*)&(Bytes[Index]);
				Value = bNormalized ? FMath::Max(((float)(*Ptr)) / 32767.f, -1.f) : *Ptr;
			}
			// UNSIGNED_SHORT
			else if (ComponentType == 5123)
			{
				uint16* Ptr = (uint16*)&(Bytes[Index]);
				Value = bNormalized ? ((float)(*Ptr)) / 65535.f : *Ptr;
			}
			else
			{
				return false;
			}

			Data.Add(Filter(Value));
		}

		return true;
	}

	template<typename T>
	bool BuildFromAccessorField(TSharedRef<FJsonObject> JsonObject, const FString Name, TArray<T>& Data, const TArray<int64> SupportedElements, const TArray<int64> SupportedTypes, const bool bNormalized)
	{
		return BuildFromAccessorField(JsonObject, Name, Data, SupportedElements, SupportedTypes, bNormalized, [&](T InValue) -> T {return InValue; });
	}

	template<typename T>
	bool BuildFromAccessorField(TSharedRef<FJsonObject> JsonObject, const FString Name, TArray<T>& Data, const TArray<int64> SupportedTypes, const bool bNormalized)
	{
		return BuildFromAccessorField(JsonObject, Name, Data, SupportedTypes, bNormalized, [&](T InValue) -> T {return InValue; });
	}

	template<int32 Num, typename T>
	bool GetJsonVector(const TArray<TSharedPtr<FJsonValue>>* JsonValues, T& Value)
	{
		if (JsonValues->Num() != Num)
			return false;

		for (int32 i = 0; i < Num; i++)
		{
			if (!(*JsonValues)[i]->TryGetNumber(Value[i]))
				return false;
		}
		
		return true;
	}
};