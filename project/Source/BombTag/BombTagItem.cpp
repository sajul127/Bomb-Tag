#include "BombTagItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BombTagCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ABombTagItem::ABombTagItem()
{
	PrimaryActorTick.bCanEverTick = false;

	// 네트워크 동기화 설정 (서버에서 지워지면 클라이언트에서도 지워짐)
	bReplicates = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetSphereRadius(50.f);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SpeedMultiplier = 1.5f;
}

void ABombTagItem::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(15.f);

	if (HasAuthority())
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABombTagItem::OnOverlapBegin);
	}
}

void ABombTagItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 반드시 서버(HasAuthority)에서만 획득 판정
	if (HasAuthority())
	{
		ABombTagCharacter* Character = Cast<ABombTagCharacter>(OtherActor);
		if (Character)
		{
			// 캐릭터에게 버프 적용 명령 (1.5배 속도, 1초 지속)
			Character->ApplySpeedBuff(1.5f, 2.0f);

			// 아이템 삭제
			Destroy();
		}
	}
}