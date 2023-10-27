// Fill out your copyright notice in the Description page of Project Settings.


#include "KillAllGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/Controller.h"
#include "ShooterAIController.h"

void AKillAllGameMode::PawnKilled(APawn* PawnKilled)
{
    Super::PawnKilled(PawnKilled);

    // Check if the player died by casting the pawn that was killed
    APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());
    if (PlayerController)
    {
        // If the player controller died, then they are not the winner
        EndGame(false);
        return;
    }

    // Loop over all enemy controllers
    for (AShooterAIController* Controller : TActorRange<AShooterAIController>(GetWorld()))
    {
        // If an enemy is still alive, then the game is not over
        if (!Controller->IsDead())
        {
            return;
        }
    }

    // All enemies are dead, player has won
    EndGame(true);
}

void AKillAllGameMode::EndGame(bool bIsPlayerWinner)
{
    // Find all controllers in the scene
    for (AController* Controller : TActorRange<AController>(GetWorld()))
    {
        Controller->GameHasEnded(Controller->GetPawn(), Controller->IsPlayerController() == bIsPlayerWinner);
    }
}