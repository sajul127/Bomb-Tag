#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BombTagItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class BOMBTAG_API ABombTagItem : public AActor
{
	GENERATED_BODY()

public:
	ABombTagItem();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	UStaticMeshComponent* MeshComponent;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// 스피드 증가량
	UPROPERTY(EditAnywhere, Category = "Item Effects")
	float SpeedMultiplier;
};