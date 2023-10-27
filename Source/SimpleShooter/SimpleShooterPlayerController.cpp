// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleShooterPlayerController.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

void ASimpleShooterPlayerController::BeginPlay()
{
    Super::BeginPlay();

    HUD = CreateWidget(this, HUDClass);
    if (HUD)
    {
        HUD->AddToViewport();
    }
}

void ASimpleShooterPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
    Super::GameHasEnded(EndGameFocus, bIsWinner);

    if (bIsWinner)
    {
        UUserWidget* WinScreen = CreateWidget(this, WinScreenClass);
        if (WinScreen)
        {
            WinScreen->AddToViewport();
        }
    }
    else
    {
        UUserWidget* LoseScreen = CreateWidget(this, LoseScreenClass);
        if (LoseScreen)
        {
            LoseScreen->AddToViewport();
        }
    }

    HUD->RemoveFromParent();

    // Restart the level in 3 seconds
    GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, 3.0F);
}