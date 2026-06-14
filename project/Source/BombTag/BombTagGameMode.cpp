#include "BombTagGameMode.h"
#include "BombTagGameState.h"
#include "BombTagCharacter.h"
#include "BombTagPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ABombTagGameMode::ABombTagGameMode()
{
	GameStateClass = ABombTagGameState::StaticClass();
	RoundTime = 15;
}

void ABombTagGameMode::BeginPlay()
{
	Super::BeginPlay();
}

// 플레이어가 접속할 때 생존자 수 동기화
void ABombTagGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ABombTagGameState* GS = GetGameState<ABombTagGameState>();
	if (GS)
	{
		GS->AlivePlayerCount = GetNumPlayers();

		// 2명 이상 모였고 아직 게임이 시작 전이라면 시작
		if (GS->AlivePlayerCount >= 2 && !GetWorldTimerManager().IsTimerActive(RoundTimerHandle))
		{
			GS->RemainingTime = RoundTime;
			AssignRandomBomber();
			GetWorldTimerManager().SetTimer(RoundTimerHandle, this, &ABombTagGameMode::UpdateRoundTimer, 1.0f, true);
		}
	}
}

void ABombTagGameMode::UpdateRoundTimer()
{
	ABombTagGameState* GS = GetGameState<ABombTagGameState>();
	if (GS)
	{
		GS->RemainingTime--;

		if (GS->RemainingTime <= 0)
		{
			GetWorldTimerManager().ClearTimer(RoundTimerHandle);
			HandleRoundEnd();
		}
	}
}

void ABombTagGameMode::HandleRoundEnd()
{
	ABombTagGameState* GS = GetGameState<ABombTagGameState>();
	if (!GS) return;

	// 현재 술래 찾아서 탈락 처리
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABombTagCharacter::StaticClass(), Players);

	for (AActor* Actor : Players)
	{
		ABombTagCharacter* Character = Cast<ABombTagCharacter>(Actor);
		if (Character && Character->bIsBomber)
		{
			Character->Eliminate();
			GS->AlivePlayerCount--;
			break;
		}
	}

	// 승패 판정
	if (GS->AlivePlayerCount <= 1)
	{
		// 최후의 1인 이름 찾기
		FString WinnerName = TEXT("Unknown");
		for (AActor* Actor : Players)
		{
			ABombTagCharacter* Character = Cast<ABombTagCharacter>(Actor);
			if (Character && !Character->bIsEliminated)
			{
				APlayerController* PC = Cast<APlayerController>(Character->GetController());
				if (PC && PC->PlayerState)
				{
					ABombTagPlayerState* PS = Cast<ABombTagPlayerState>(PC->PlayerState);
					if (PS && !PS->PlayerNickname.IsEmpty())
					{
						WinnerName = PS->PlayerNickname;
					}
					else
					{
						WinnerName = PC->PlayerState->GetPlayerName();
					}
				}
				break;
			}
		}

		// 1. GameState에게 모두에게 승자를 알리라고 명령!
		GS->Multicast_GameOver(WinnerName);

		// 2. 5초 뒤 대기방으로 다 같이 자동 이동
		FTimerHandle ReturnTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ReturnTimerHandle, [this]()
			{
				GetWorld()->ServerTravel(TEXT("/Game/stuff/Map/WaitingRoomMap?listen"));
			}, 5.0f, false);
	}
	else
	{
		// 2초 뒤 다음 라운드 시작
		FTimerHandle NextRoundTimer;
		GetWorldTimerManager().SetTimer(NextRoundTimer, [this, GS]()
			{
				GS->RemainingTime = RoundTime;
				AssignRandomBomber();
				GetWorldTimerManager().SetTimer(RoundTimerHandle, this, &ABombTagGameMode::UpdateRoundTimer, 1.0f, true);
			}, 2.0f, false);
	}
}

void ABombTagGameMode::AssignRandomBomber()
{
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABombTagCharacter::StaticClass(), Players);

	TArray<ABombTagCharacter*> AlivePlayers;
	for (AActor* Actor : Players)
	{
		ABombTagCharacter* Character = Cast<ABombTagCharacter>(Actor);
		if (Character && !Character->bIsEliminated)
		{
			Character->bIsBomber = false; // 초기화
			AlivePlayers.Add(Character);
		}
	}

	if (AlivePlayers.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, AlivePlayers.Num() - 1);
		AlivePlayers[RandomIndex]->bIsBomber = true;
		AlivePlayers[RandomIndex]->OnRep_bIsBomber(); // 이펙트 갱신
	}
}

void ABombTagGameMode::Multicast_GameOver_Implementation(const FString& WinnerName) {
		// 게임 오버 UI 표시 
	UE_LOG(LogTemp, Log, TEXT("Game Over! Winner: %s"), *WinnerName);
}