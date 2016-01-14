// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzlePreset.h"
#include "PuzzlePresetGameInstance.h"

const FName UPuzzlePresetGameInstance::SubsystemName = FName("Null");
#define SUBSYSTEM_NAME UPuzzlePresetGameInstance::SubsystemName
#define GET_ID(User) User->GetPreferredUniqueNetId()

UPuzzlePresetGameInstance::UPuzzlePresetGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	/** Bind function for CREATING a Session */
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UPuzzlePresetGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UPuzzlePresetGameInstance::OnStartOnlineGameComplete);
	/** Bind function for FINDING a Session */
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UPuzzlePresetGameInstance::OnFindSessionsComplete);
	/** Bind function for JOINING a Session */
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UPuzzlePresetGameInstance::OnJoinSessionComplete);
	/** Bind function for DESTROYING a Session */
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UPuzzlePresetGameInstance::OnDestroySessionComplete);
}

#pragma region C++ only Methods
bool UPuzzlePresetGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	// Get the Online Subsystem to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get(SUBSYSTEM_NAME);

	if (OnlineSub)
	{
		// Get the Session Interface, so we can call the "CreateSession" function on it
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();

		if (Session.IsValid() && UserId.IsValid())
		{
			/*
			Fill in all the Session Settings that we want to use.

			There are more with SessionSettings.Set(...);
			For example the Map or the GameMode/Type.
			*/
			SessionSettings = MakeShareable(new FOnlineSessionSettings());

			SessionSettings->bIsLANMatch = bIsLAN;
			SessionSettings->bUsesPresence = bIsPresence;
			SessionSettings->NumPublicConnections = MaxNumPlayers;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

			//SessionSettings->Set(SETTING_MAPNAME, FString("/Game/PuzzleCPP/Maps/PuzzleExampleMap"), EOnlineDataAdvertisementType::ViaOnlineService);
			SessionSettings->Set(SETTING_MAPNAME, FString("PuzzleExampleMap"), EOnlineDataAdvertisementType::ViaOnlineService);

			// Set the delegate to the Handle of the SessionInterface
			OnCreateSessionCompleteDelegateHandle = Session->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

			// Our delegate should get called when this is complete (doesn't need to be successful!)
			return Session->CreateSession(*UserId, SessionName, *SessionSettings);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Session valid: %d; UserId valid: %d"), Session.IsValid(), UserId.IsValid()));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
	}

	return false;
}

void UPuzzlePresetGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence)
{
	bool bInvalidate = true;

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SUBSYSTEM_NAME);

	if (OnlineSub)
	{
		// Get the SessionInterface from our OnlineSubsystem
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();

		if (Session.IsValid() && UserId.IsValid())
		{
			bInvalidate = false;
			/*
			Fill in all the SearchSettings, like if we are searching for a LAN game and how many results we want to have!
			*/
			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			SessionSearch->bIsLanQuery = bIsLAN;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->PingBucketSize = 50;
			SessionSearch->TimeoutInSeconds = 10;

			// We only want to set this Query Setting if "bIsPresence" is true
			if (bIsPresence)
			{
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
			}

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

			// Set the Delegate to the Delegate Handle of the FindSession function
			OnFindSessionsCompleteDelegateHandle = Session->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

			// Finally call the SessionInterface function. The Delegate gets called once this is finished
			Session->FindSessions(*UserId, SearchSettingsRef);
		}
	}
	if (bInvalidate)
	{
		// If something goes wrong, just call the Delegate Function directly with "false".
		OnFindSessionsComplete(false);
	}
}

bool UPuzzlePresetGameInstance::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	// Return bool
	bool bSuccessful = false;

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SUBSYSTEM_NAME);

	if (OnlineSub)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();

		if (Session.IsValid() && UserId.IsValid())
		{
			// Set the Handle again
			OnJoinSessionCompleteDelegateHandle = Session->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
			bSuccessful = Session->JoinSession(*UserId, SessionName, SearchResult);
		}
	}

	return bSuccessful;
}
#pragma endregion

#pragma region Blueprint callable Methods
void UPuzzlePresetGameInstance::StartOnlineGame()
{
	// Creating a local player where we can get userId from
	//
	ULocalPlayer* const LocalPlayer = GetFirstGamePlayer();

	//HostSession(LocalPlayer->GetCachedUniqueNetId(), GameSessionName, true, true, 2);
	HostSession(GET_ID(LocalPlayer), GameSessionName, true, true, 2);
}

void UPuzzlePresetGameInstance::FindOnlineGames()
{
	ULocalPlayer* const LocalPlayer = GetFirstGamePlayer();
	UE_LOG(LogTemp, Warning, TEXT("Start to trying find some sessions. Our Id valid: %d"), GET_ID(LocalPlayer)->IsValid());

	FindSessions(GET_ID(LocalPlayer), GameSessionName, true, true);
}

void UPuzzlePresetGameInstance::JoinOnlineGame()
{
	ULocalPlayer* const LocalPlayer = GetFirstGamePlayer();

	// settings instance in case we fine more then one
	FOnlineSessionSearchResult SearchResult;

	// if array isn't empty then go through
	if (SessionSearch->SearchResults.Num() > 0)
	{
		for (uint8 i = 0; i < SessionSearch->SearchResults.Num(); i++)
		{
			if (SessionSearch->SearchResults[i].Session.OwningUserId != GET_ID(LocalPlayer))
			{
				SearchResult = SessionSearch->SearchResults[i];

				// Once we found sounce a Session that is not ours, just join it. Instead of using a for loop, you could
				// use a widget where you click on and have a reference for the GameSession it represents which you can use
				// here
				JoinSession(GET_ID(LocalPlayer), GameSessionName, SearchResult);
				break;
			}
		}
	}
}

void UPuzzlePresetGameInstance::DestroySessionAndLeaveGame()
{
	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get(SUBSYSTEM_NAME);
	if (SubSystem)
	{
		IOnlineSessionPtr Session = SubSystem->GetSessionInterface();
		if (Session.IsValid())
		{
			OnDestroySessionCompleteDelegateHandle = Session->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
			Session->DestroySession(GameSessionName);
		}
	}
}
#pragma endregion

#pragma region Event handlers
void UPuzzlePresetGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the OnlineSubsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SUBSYSTEM_NAME);
	if (OnlineSub)
	{
		// Get the Session Interface to call the StartSession function
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();

		if (Session.IsValid())
		{
			// Clear the SessionComplete delegate handle, since we finished this call
			Session->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				// Set the StartSession delegate handle
				OnStartSessionCompleteDelegateHandle = Session->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				// Our StartSessionComplete delegate should get called after this
				Session->StartSession(SessionName);
			}
		}
	}
}

void UPuzzlePresetGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the Online Subsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SUBSYSTEM_NAME);
	if (OnlineSub)
	{
		// Get the Session Interface to clear the Delegate
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			// Clear the delegate, since we are done with this call
			Session->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}

	// If the start was successful, we can open a NewMap if we want. Make sure to use "listen" as a parameter!
	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), "PuzzleExampleMap", true, "listen");
	}
}

void UPuzzlePresetGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	bool findSomething = false;
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionsComplete bSuccess: %d"), bWasSuccessful);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnFindSessionsComplete bSuccess: %d"), bWasSuccessful));

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get(SUBSYSTEM_NAME);
	if (bWasSuccessful && OnlineSub)
	{
		// Get SessionInterface of the OnlineSubsystem
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			// Clear the Delegate handle, since we finished this call
			Session->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			// Just debugging the Number of Search results. Can be displayed in UMG or something later on
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num()));

			// If we have found at least 1 session, we just going to debug them. You could add them to a list of UMG Widgets, like it is done in the BP version!
			if (SessionSearch->SearchResults.Num() > 0)
			{
				// "SessionSearch->SearchResults" is an Array that contains all the information. You can access the Session in this and get a lot of information.
				// This can be customized later on with your own classes to add more information that can be set and displayed
				for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
				{
					// OwningUserName is just the SessionName for now. I guess you can create your own Host Settings class and GameSession Class and add a proper GameServer Name here.
					// This is something you can't do in Blueprint for example!
					UE_LOG(LogTemp, Warning, TEXT("Session Number: %d | Sessionname: %s "), SearchIdx + 1, *(SessionSearch->SearchResults[SearchIdx].Session.OwningUserName));
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Session Number: %d | Sessionname: %s "), SearchIdx + 1, *(SessionSearch->SearchResults[SearchIdx].Session.OwningUserName)));
				}

				findSomething = true;
			}
		}
	}

	OnFindSessionCompleteDelegateBP.Broadcast(findSomething);
}

void UPuzzlePresetGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnJoinSessionComplete %s, %d"), *SessionName.ToString(), static_cast<int32>(Result)));

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SUBSYSTEM_NAME);
	if (OnlineSub)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();

		if (Session.IsValid())
		{
			// Clear the Delegate again
			Session->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

			// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
			// This is something the Blueprint Node "Join Session" does automatically!
			APlayerController * const PlayerController = GetFirstLocalPlayerController();

			// We need a FString to use ClientTravel and we can let the SessionInterface contruct such a
			// String for us by giving him the SessionName and an empty String. We want to do this, because
			// Every OnlineSubsystem uses different TravelURLs
			FString TravelURL;

			if (PlayerController && Session->GetResolvedConnectString(SessionName, TravelURL))
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Begin travel on url \"%s\""), *TravelURL));

				// Finally call the ClienTravel. If you want, you could print the TravelURL to see
				// how it really looks like
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UPuzzlePresetGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnDestroySessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(SUBSYSTEM_NAME);
	if (OnlineSub)
	{
		// Get the SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();

		if (Session.IsValid())
		{
			// Clear the Delegate
			Session->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

			// If it was successful, we just load another level (could be a MainMenu!)
			if (bWasSuccessful)
			{
				UGameplayStatics::OpenLevel(GetWorld(), "MainMenuMap", true);
			}
		}
	}
}
#pragma endregion
#undef SUBSYSTEM_NAME