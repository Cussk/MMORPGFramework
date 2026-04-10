// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

/**
 * Lightweight framework-wide component lookup helpers.
 *
 * Architectural role:
 * - Centralize common "find my framework component" patterns.
 * - Reduce repeated casts and repeated lookup boilerplate.
 * - Support a composition-first framework style where systems are attached to
 *   common engine classes instead of requiring deep subclass hierarchies.
 *
 * Why a plain C++ helper instead of another UObject type:
 * - No state is owned here.
 * - No reflection is needed.
 * - This is just a lightweight access pattern utility.
 */
class MDFFRAMEWORKCORE_API FMDFComponentHelpers
{
public:
	template <typename TComponent>
	static TComponent* FindOnActor(AActor* Actor)
	{
		return Actor ? Actor->FindComponentByClass<TComponent>() : nullptr;
	}

	template <typename TComponent>
	static const TComponent* FindOnActor(const AActor* Actor)
	{
		return Actor ? Actor->FindComponentByClass<TComponent>() : nullptr;
	}

	template <typename TComponent>
	static TComponent* FindOnPlayerState(APlayerState* PlayerState)
	{
		return FindOnActor<TComponent>(PlayerState);
	}

	template <typename TComponent>
	static const TComponent* FindOnPlayerState(const APlayerState* PlayerState)
	{
		return FindOnActor<TComponent>(PlayerState);
	}

	template <typename TComponent>
	static TComponent* FindFromController(AController* Controller)
	{
		if (!Controller)
		{
			return nullptr;
		}

		if (TComponent* FoundOnController = FindOnActor<TComponent>(Controller))
		{
			return FoundOnController;
		}

		return FindOnPlayerState<TComponent>(Controller->GetPlayerState<APlayerState>());
	}

	template <typename TComponent>
	static const TComponent* FindFromController(const AController* Controller)
	{
		if (!Controller)
		{
			return nullptr;
		}

		if (const TComponent* FoundOnController = FindOnActor<TComponent>(Controller))
		{
			return FoundOnController;
		}

		return FindOnPlayerState<TComponent>(Controller->GetPlayerState<APlayerState>());
	}

	template <typename TComponent>
	static TComponent* FindFromPawn(APawn* Pawn)
	{
		if (!Pawn)
		{
			return nullptr;
		}

		if (TComponent* FoundOnPawn = FindOnActor<TComponent>(Pawn))
		{
			return FoundOnPawn;
		}

		if (AController* Controller = Pawn->GetController())
		{
			if (TComponent* FoundFromController = FindFromController<TComponent>(Controller))
			{
				return FoundFromController;
			}
		}

		return FindOnPlayerState<TComponent>(Pawn->GetPlayerState());
	}

	template <typename TComponent>
	static const TComponent* FindFromPawn(const APawn* Pawn)
	{
		if (!Pawn)
		{
			return nullptr;
		}

		if (const TComponent* FoundOnPawn = FindOnActor<TComponent>(Pawn))
		{
			return FoundOnPawn;
		}

		if (const AController* Controller = Pawn->GetController())
		{
			if (const TComponent* FoundFromController = FindFromController<TComponent>(Controller))
			{
				return FoundFromController;
			}
		}

		return FindOnPlayerState<TComponent>(Pawn->GetPlayerState());
	}
};