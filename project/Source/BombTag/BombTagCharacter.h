#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BombTagCharacter.generated.h"

UCLASS()
class BOMBTAG_API ABombTagCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABombTagCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* --- 폭탄 및 상태 변수 --- */

	// 현재 술래(폭탄 소지자)인지 여부 (RepNotify를 통해 변경 시 이펙트 자동 업데이트)
	UPROPERTY(ReplicatedUsing = OnRep_bIsBomber, BlueprintReadOnly, Category = "Bomb")
	bool bIsBomber;

	// 폭탄 전달 후 즉시 다시 받는 핑퐁 현상을 막기 위한 무적 쿨타임 여부
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Bomb")
	bool bIsInvincible;

	// 변수가 동기화될 때 호출되는 C++ 함수
	UFUNCTION()
	void OnRep_bIsBomber();

	// 블루프린트에서 폭탄 켜고 끄기를 구현할 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Bomb")
	void UpdateBombVisuals(bool bIsNowBomber);

	// 충돌(Overlap) 이벤트 바인딩
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 서버로 폭탄 전달 요청을 보내는 RPC
	UFUNCTION(Server, Reliable)
	void Server_PassBomb(ABombTagCharacter* Target);

	// 타이머 핸들 (무적 해제용)
	FTimerHandle InvincibleTimerHandle;

	// 무적 상태 해제
	void ClearInvincibility();

	// 폭탄 폭발 시 사망 처리 (GameMode에서 호출)
	void Eliminate();

	// 사망 이펙트를 모든 클라이언트에게 재생
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayExplosionEffect();

	// 블루프린트에서 폭발 이펙트를 터뜨릴 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Bomb")
	void PlayExplosionVisuals();

	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bIsEliminated;

	// 캐릭터의 속도를 되돌리기 위한 함수 선언
	UFUNCTION()
	void ResetSpeed();

	UFUNCTION()
	void ApplySpeedBuff(float Multiplier, float Duration);

	// 원래 속도를 저장해둘 변수
	float DefaultWalkSpeed;
};