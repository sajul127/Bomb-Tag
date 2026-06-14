#include "BombTagCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

ABombTagCharacter::ABombTagCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsBomber = false;
	bIsInvincible = false;

	// Overlap 이벤트 활성화
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
}

void ABombTagCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 클라이언트 자신의 캡슐 컴포넌트에 Overlap 이벤트 바인딩
	if (IsLocallyControlled() || HasAuthority())
	{
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABombTagCharacter::OnOverlapBegin);
	}
}

void ABombTagCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABombTagCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABombTagCharacter, bIsBomber);
	DOREPLIFETIME(ABombTagCharacter, bIsEliminated);
	DOREPLIFETIME(ABombTagCharacter, bIsInvincible);
}

void ABombTagCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 내가 술래이고, 로컬 컨트롤러이며, 상대가 다른 플레이어일 때만 처리
	if (IsLocallyControlled() && bIsBomber && !bIsInvincible)
	{
		ABombTagCharacter* TargetCharacter = Cast<ABombTagCharacter>(OtherActor);
		if (TargetCharacter && TargetCharacter != this && !TargetCharacter->bIsInvincible)
		{
			// 서버로 전달 요청
			Server_PassBomb(TargetCharacter);
		}
	}
}

void ABombTagCharacter::Server_PassBomb_Implementation(ABombTagCharacter* Target)
{
	// 서버 측 한 번 더 검증 (해킹 방지 및 상태 확인)
	if (!Target || !bIsBomber || bIsInvincible || Target->bIsInvincible) return;

	//  Lag (핑 차이) 보정 로직
	// 실제 캐릭터 충돌 반경 합보다 30% 정도 넓게 판정하여 억울한 상황 방지
	float MyRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	float TargetRadius = Target->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float AllowedDistance = (MyRadius + TargetRadius) * 1.3f; // 30% Tolerance 허용 오차

	float ActualDistance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());

	if (ActualDistance <= AllowedDistance)
	{
		// 폭탄 소유권 이전
		bIsBomber = false;
		Target->bIsBomber = true;

		// 핑퐁 현상 방지를 위한 무적(쿨타임) 1.5초 적용
		bIsInvincible = true;
		Target->bIsInvincible = true;

		GetWorldTimerManager().SetTimer(InvincibleTimerHandle, this, &ABombTagCharacter::ClearInvincibility, 1.5f, false);
		Target->GetWorldTimerManager().SetTimer(Target->InvincibleTimerHandle, Target, &ABombTagCharacter::ClearInvincibility, 1.5f, false);

		// 서버에서도 이펙트 업데이트 함수 명시적 호출
		OnRep_bIsBomber();
		Target->OnRep_bIsBomber();
	}
}

void ABombTagCharacter::ClearInvincibility()
{
	bIsInvincible = false;
}

void ABombTagCharacter::OnRep_bIsBomber()
{
	UpdateBombVisuals(bIsBomber);
}

void ABombTagCharacter::Eliminate()
{
	if (HasAuthority())
	{
		Multicast_PlayExplosionEffect();
		// 사망 처리: 충돌 해제, 보이지 않게 처리
		SetActorHiddenInGame(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->DisableMovement();
		bIsEliminated = true;
	}
}

void ABombTagCharacter::Multicast_PlayExplosionEffect_Implementation()
{
	PlayExplosionVisuals();
}

// 아이템 획득 시 호출할 함수 (아이템 클래스에서 이 함수를 호출하세요)
void ABombTagCharacter::ApplySpeedBuff(float Multiplier, float Duration)
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp)
	{
		// 원래 속도 저장 (첫 획득 시에만)
		if (DefaultWalkSpeed <= 0.f) DefaultWalkSpeed = MovementComp->MaxWalkSpeed;

		// 속도 증가
		MovementComp->MaxWalkSpeed *= Multiplier;

		// 1초(Duration) 뒤에 ResetSpeed 함수 실행
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ABombTagCharacter::ResetSpeed, Duration, false);
	}
}

void ABombTagCharacter::ResetSpeed()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp && DefaultWalkSpeed > 0.f)
	{
		// 원래 속도로 복구
		MovementComp->MaxWalkSpeed = DefaultWalkSpeed;
	}
}