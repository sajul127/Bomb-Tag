#include "GI_GameInstance.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"
#include "FindSessionsCallbackProxy.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"

void UGI_GameInstance::Init()
{
	Super::Init();
	// 게임 시작 시 초기화
	bFirebaseLoggedIn = false;
	bKickedFromSession = false;

	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UGI_GameInstance::OnNetworkFailure);
	}
}

void UGI_GameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	// 방 폭파 시 튕겼음을 기록!
	bKickedFromSession = true;

	// 클라이언트가 강제로 튕겼을 때, 로컬에 남아있는 세션 찌꺼기를 파기해야 다시 다른 방에 들어갈 수 있습니다.
	if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld()))
	{
		if (IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface())
		{
			SessionInterface->DestroySession(NAME_GameSession);
		}
	}
}

void UGI_GameInstance::ReturnToMainMenu()
{
	// 방 폭파 등의 이유로 서버와의 연결이 끊어지면 실행됨
	bKickedFromSession = true;

	if (GEngine && GetWorld())
	{
		FString ErrorMsg;
		// 프로젝트 세팅의 GameDefaultMap(현재 TitleMap) 대신 로비맵으로 강제 이동
		GEngine->Browse(GEngine->GetWorldContextFromWorldChecked(GetWorld()), FURL(nullptr, TEXT("/Game/stuff/Map/LobbyMap"), TRAVEL_Absolute), ErrorMsg);
	}
}

// -------------------------------------------------------------------------
// 회원가입 (Sign Up) 로직
// -------------------------------------------------------------------------
void UGI_GameInstance::SignUpUser(FString Email, FString Password)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	FString Url = TEXT("https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=") + FirebaseApiKey;
	Request->SetURL(Url);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField("email", Email);
	JsonObject->SetStringField("password", Password);
	JsonObject->SetBoolField("returnSecureToken", true);

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(JsonString);
	Request->OnProcessRequestComplete().BindUObject(this, &UGI_GameInstance::OnSignUpResponseReceived);
	Request->ProcessRequest();
}

void UGI_GameInstance::OnSignUpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		FString ResponseString = Response->GetContentAsString();

		if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
		{
			UE_LOG(LogTemp, Log, TEXT("Firebase Sign Up Success!"));
			ParseAndSaveToken(ResponseString);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Firebase Sign Up Failed. Code: %d, Response: %s"), Response->GetResponseCode(), *ResponseString);
		}
	}
}

// -------------------------------------------------------------------------
// 로그인 (Sign In) 로직
// -------------------------------------------------------------------------
void UGI_GameInstance::SignInUser(FString Email, FString Password)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	FString Url = TEXT("https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=") + FirebaseApiKey;
	Request->SetURL(Url);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField("email", Email);
	JsonObject->SetStringField("password", Password);
	JsonObject->SetBoolField("returnSecureToken", true);

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(JsonString);
	Request->OnProcessRequestComplete().BindUObject(this, &UGI_GameInstance::OnSignInResponseReceived);
	Request->ProcessRequest();
}

void UGI_GameInstance::OnSignInResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		FString ResponseString = Response->GetContentAsString();

		if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
		{
			UE_LOG(LogTemp, Log, TEXT("Firebase Sign In Success!"));
			ParseAndSaveToken(ResponseString);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Firebase Sign In Failed. Code: %d, Response: %s"), Response->GetResponseCode(), *ResponseString);
			bFirebaseLoggedIn = false;
		}
	}
}

void UGI_GameInstance::UpdateNickname(FString Nickname)
{
	// 로그인이 안 되어있거나 토큰이 없으면 막음
	if (!bFirebaseLoggedIn || FirebaseToken.IsEmpty()) return;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	// 프로필 업데이트 전용 URL
	FString Url = TEXT("https://identitytoolkit.googleapis.com/v1/accounts:update?key=") + FirebaseApiKey;
	Request->SetURL(Url);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField("idToken", FirebaseToken); // 발급받은 토큰 사용
	JsonObject->SetStringField("displayName", Nickname);  // 닉네임 지정
	JsonObject->SetBoolField("returnSecureToken", false);

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(JsonString);
	Request->OnProcessRequestComplete().BindUObject(this, &UGI_GameInstance::OnUpdateNicknameResponseReceived);
	Request->ProcessRequest();
}

void UGI_GameInstance::OnUpdateNicknameResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		UE_LOG(LogTemp, Log, TEXT("Nickname Update Success!"));

		// 업데이트 성공 시 응답에서 닉네임을 다시 빼서 변수에 저장해줍니다.
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			FirebaseNickname = JsonObject->GetStringField("displayName");
			UE_LOG(LogTemp, Log, TEXT("Current Nickname is: %s"), *FirebaseNickname);
		}
	}
}

// === 4. 게스트 로그인 (익명) ===
void UGI_GameInstance::SignInAnonymously()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	// 익명 로그인 엔드포인트 URL
	FString Url = TEXT("https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=") + FirebaseApiKey;
	Request->SetURL(Url);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");

	// 익명 로그인은 본문에 returnSecureToken만 true로 보내면 됩니다.
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetBoolField("returnSecureToken", true);

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(JsonString);
	Request->OnProcessRequestComplete().BindUObject(this, &UGI_GameInstance::OnSignInAnonymouslyResponseReceived);
	Request->ProcessRequest();
}

void UGI_GameInstance::OnSignInAnonymouslyResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		UE_LOG(LogTemp, Log, TEXT("Firebase Guest Sign In Success!"));
		ParseAndSaveToken(Response->GetContentAsString());

		if (!FirebaseNickname.IsEmpty())
		{
			UpdateNickname(FirebaseNickname);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Firebase Guest Sign In Failed."));
		bFirebaseLoggedIn = false;
	}
}

// -------------------------------------------------------------------------
// 성공 시 토큰 저장 로직 (공통)
// -------------------------------------------------------------------------
void UGI_GameInstance::ParseAndSaveToken(const FString& ResponseString)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		FirebaseToken = JsonObject->GetStringField("idToken");
		FirebaseUid = JsonObject->GetStringField("localId");

		if (JsonObject->HasField("displayName") && !JsonObject->GetStringField("displayName").IsEmpty())
		{
			FirebaseNickname = JsonObject->GetStringField("displayName");
		}
		else if (FirebaseNickname.IsEmpty())
		{
			FirebaseNickname = TEXT("Guest");
		}

		bFirebaseLoggedIn = true;

		UE_LOG(LogTemp, Log, TEXT("Token Saved! Logged In. UID: %s"), *FirebaseUid);
	}
}
FString UGI_GameInstance::GetHostNameFromSession(const FBlueprintSessionResult& SessionResult)
{
	FString HostName;
	if (SessionResult.OnlineResult.Session.SessionSettings.Get(FName("HostNickname"), HostName))
	{
		return HostName;
	}
	return SessionResult.OnlineResult.Session.OwningUserName;
}


void UGI_GameInstance::UpdateSessionHostName(FString Nickname)
{
	UE_LOG(LogTemp, Warning, TEXT("UpdateSessionHostName called with Nickname: %s"), *Nickname);
	if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld()))
	{
		if (IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface())
		{
			FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(NAME_GameSession);
			if (NamedSession)
			{
				UE_LOG(LogTemp, Warning, TEXT("Settings found! Setting HostNickname"));
				NamedSession->SessionSettings.Set(FName("HostNickname"), Nickname, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
				SessionInterface->UpdateSession(NAME_GameSession, NamedSession->SessionSettings, true);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Settings NOT found for NAME_GameSession!"));
			}
		}
	}
}
