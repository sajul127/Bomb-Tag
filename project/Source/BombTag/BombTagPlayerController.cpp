// Copyright Epic Games, Inc. All Rights Reserved.


#include "BombTagPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "BombTag.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "GameFramework/PlayerState.h"
#include "GI_GameInstance.h"
#include "BombTagPlayerState.h"

void ABombTagPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		if (UGI_GameInstance* GI = Cast<UGI_GameInstance>(GetGameInstance()))
		{
			Server_SetNickname(GI->FirebaseNickname);
		}
	}

	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogBombTag, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
	if (IsLocalPlayerController() && MainUIClass != nullptr)
	{
		MainUIInstance = CreateWidget<UUserWidget>(this, MainUIClass);
		if (MainUIInstance != nullptr)
		{
			MainUIInstance->AddToViewport();
			bShowMouseCursor = true;

			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(MainUIInstance->TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputModeData);
		}
	}
}

void ABombTagPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool ABombTagPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void ABombTagPlayerController::Server_SetNickname_Implementation(const FString& InNickname)
{
	UE_LOG(LogTemp, Log, TEXT("Server_SetNickname called with: %s"), *InNickname);
	if (ABombTagPlayerState* PS = GetPlayerState<ABombTagPlayerState>())
	{
		PS->PlayerNickname = InNickname;
	}
}
