// Thang Phung Dinh, 2016, Fog of War implementation for Jump Step Step game
// Feel free to use this  in your game

#pragma once

#include "GameFramework/Actor.h"
#include "AProFow.generated.h"

UCLASS()
class POTL_API AProFow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProFow();

	virtual void PostInitializeComponents() override;
	
	// Set the fog size
	void setSize(float);

	// Reveal a portion of the fow
	//UFUNCTION(BlueprintCallable)
	void revealSmoothCircle(const FVector2D &pos, float radius);

private:

	void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);

	// Fow texture size
	static const int m_textureSize = 512;

	UPROPERTY()
	UStaticMeshComponent* m_squarePlane;
	UPROPERTY()
	UTexture2D* m_dynamicTexture;
	UPROPERTY()
	UMaterialInterface* m_dynamicMaterial;
	UPROPERTY()
	UMaterialInstanceDynamic *m_dynamicMaterialInstance;
	uint8 m_pixelArray[m_textureSize * m_textureSize];
	FUpdateTextureRegion2D m_wholeTextureReagion;
	float m_coverSize;
};
