#include "StfSessionLibrary.h"

#include "LatentActions.h"
#include "OnlineSubsystemUtils.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

class FCreateSessionAction final : public FPendingLatentAction
{
	bool Completed = false;
	bool& SuccessPrivate;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	IOnlineSessionPtr SessionsPrivate;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	void OnCreateSessionComplete(FName SessionName, const bool bWasSuccessful)
	{
		SessionsPrivate->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);

		if (!bWasSuccessful)
		{
			SuccessPrivate = false;
			Completed = true;
			return;
		}

		FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
		OnStartSessionCompleteDelegate.BindRaw(this, &FCreateSessionAction::OnStartSessionComplete);
		OnStartSessionCompleteDelegateHandle = SessionsPrivate->AddOnStartSessionCompleteDelegate_Handle(
			OnStartSessionCompleteDelegate
		);

		SessionsPrivate->StartSession(NAME_GameSession);
	}

	void OnStartSessionComplete(FName SessionName, const bool bWasSuccessful)
	{
		SessionsPrivate->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);

		SuccessPrivate = bWasSuccessful;
		Completed = true;
	}

public:
	FCreateSessionAction(
		const APlayerController* PlayerController,
		const FStfOnlineSessionSettings& OnlineSessionSettings,
		bool& Success,
		const FLatentActionInfo& LatentInfo
	): SuccessPrivate(Success),
	   ExecutionFunction(LatentInfo.ExecutionFunction),
	   OutputLink(LatentInfo.Linkage),
	   CallbackTarget(LatentInfo.CallbackTarget)
	{
		SessionsPrivate = Online::GetSubsystem(
			GEngine->GetWorldFromContextObject(PlayerController, EGetWorldErrorMode::ReturnNull), NAME_None
		)->GetSessionInterface();

		FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
		OnCreateSessionCompleteDelegate.BindRaw(this, &FCreateSessionAction::OnCreateSessionComplete);
		OnCreateSessionCompleteDelegateHandle = SessionsPrivate->AddOnCreateSessionCompleteDelegate_Handle(
			OnCreateSessionCompleteDelegate
		);

		FOnlineSessionSettings Settings;
		Settings.NumPublicConnections = OnlineSessionSettings.NumPublicConnections;
		Settings.bShouldAdvertise = true;
		Settings.bAllowJoinInProgress = true;
		Settings.bIsLANMatch = false;
		Settings.bUsesPresence = true;
		Settings.bAllowJoinViaPresence = true;
		Settings.bAllowInvites = true;
		Settings.bUseLobbiesIfAvailable = true;

		SessionsPrivate->CreateSession(
			*PlayerController->PlayerState->GetUniqueId().GetUniqueNetId(),
			NAME_GameSession,
			Settings
		);
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		Response.FinishAndTriggerIf(Completed, ExecutionFunction, OutputLink, CallbackTarget);
	}

#if WITH_EDITOR
	virtual FString GetDescription() const override
	{
		return "CreateSession";
	}
#endif
};

void UStfSessionLibrary::CreateSession(
	UObject* WorldContextObject,
	APlayerController* PlayerController,
	const FStfOnlineSessionSettings Settings,
	bool& Success,
	FLatentActionInfo LatentInfo
)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		if (
			!LatentActionManager.FindExistingAction<FCreateSessionAction>(LatentInfo.CallbackTarget, LatentInfo.UUID)
		)
		{
			LatentActionManager.AddNewAction(
				LatentInfo.CallbackTarget,
				LatentInfo.UUID,
				new FCreateSessionAction(PlayerController, Settings, Success, LatentInfo)
			);
		}
	}
}

FStfDelegateHandle UStfSessionLibrary::AddOnSessionUserInviteAccepted(
	const UObject* WorldContextObject,
	FStfOnSessionUserInviteAcceptedDelegate Event
)
{
	const IOnlineSessionPtr Sessions = Online::GetSubsystem(
		GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull), NAME_None
	)->GetSessionInterface();

	FOnSessionUserInviteAcceptedDelegate OnSessionUserInviteAcceptedDelegate;

	OnSessionUserInviteAcceptedDelegate.BindLambda(
		[Event]
	(const bool bWasSuccessful, const int32, FUniqueNetIdPtr, const FOnlineSessionSearchResult& InviteResult)
		{
			Event.Execute(bWasSuccessful, {InviteResult});
		}
	);

	return {
		Sessions->AddOnSessionUserInviteAcceptedDelegate_Handle(OnSessionUserInviteAcceptedDelegate)
	};
}

void UStfSessionLibrary::ClearOnSessionUserInviteAccepted(
	const UObject* WorldContextObject,
	FStfDelegateHandle Handle
)
{
	const IOnlineSessionPtr Sessions = Online::GetSubsystem(
		GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull), NAME_None
	)->GetSessionInterface();

	Sessions->ClearOnSessionUserInviteAcceptedDelegate_Handle(Handle.Handle);
}
