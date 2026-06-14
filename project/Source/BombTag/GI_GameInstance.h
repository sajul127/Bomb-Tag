#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/IHttpRequest.h"
#include "GI_GameInstance.generated.h"

UCLASS()
class BOMBTAG_API UGI_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 프로젝트 셋업 단계에서 초기화
	virtual void Init() override;

	// 방이 폭파되거나 튕겼을 때 로비 맵으로 강제 이동시키기 위한 오버라이드
	virtual void ReturnToMainMenu() override;

	// 엔진의 네트워크 끊김 이벤트를 가로채서 상태를 기록하는 함수
	void OnNetworkFailure(UWorld* World, class UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

	const FString FirebaseApiKey = TEXT("AIzaSyAeLuVkRKxGqfJK6n39VfIzIbgsmrR2s7w");

	// 블루프린트(위젯)에서 호출할 회원가입 함수
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth")
	void SignUpUser(FString Email, FString Password);

	// 블루프린트(위젯)에서 호출할 로그인 함수
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth")
	void SignInUser(FString Email, FString Password);

	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth")
	void UpdateNickname(FString Nickname);

	// 세션의 방장 닉네임 가져오기
	UFUNCTION(BlueprintPure, Category = "Session")
	FString GetHostNameFromSession(const struct FBlueprintSessionResult& SessionResult);

	// 세션 정보에 내 닉네임 기록하기 (방장 전용)
	UFUNCTION(BlueprintCallable, Category = "Session")
	void UpdateSessionHostName(FString Nickname);

	// 현재 로그인 상태인지 확인
	UPROPERTY(BlueprintReadOnly, Category = "Firebase|User")
	bool bFirebaseLoggedIn = false;

	// 방 폭파 등으로 튕겨서 로비로 돌아왔는지 여부를 확인하는 변수
	UPROPERTY(BlueprintReadWrite, Category = "Session")
	bool bKickedFromSession = false;

	// Firebase에서 발급해준 보안 토큰
	UPROPERTY(BlueprintReadOnly, Category = "Firebase|User")
	FString FirebaseToken;

	// Firebase에서 발급해준 유저의 고유 식별 번호
	UPROPERTY(BlueprintReadOnly, Category = "Firebase|User")
	FString FirebaseUid;

	UPROPERTY(BlueprintReadWrite, Category = "Firebase|User")
	FString FirebaseNickname;

	// 게스트 로그인 함수
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth")
	void SignInAnonymously();

private:
	// 서버의 응답을 받을 콜백 함수들
	void OnSignUpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void OnSignInResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void OnUpdateNicknameResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// 익명 로그인 응답 콜백
	void OnSignInAnonymouslyResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	// ... 기존 코드 ...

	// JSON 응답을 파싱하여 토큰을 저장하는 공통 함수
	void ParseAndSaveToken(const FString& ResponseString);
};