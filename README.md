#  BombTag (폭탄 피하기 멀티플레이 게임)

BombTag는 언리얼 엔진 5(Unreal Engine 5)를 기반으로 제작된 멀티플레이어 폭탄 돌리기 게임입니다. 
Firebase REST API를 활용한 사용자 인증 시스템과 언리얼 엔진의 OnlineSubsystem을 활용한 로비 및 세션 매치메이킹 시스템이 완벽하게 구현되어 있습니다.

##  기술 스택 (Tech Stack)
* **Game Engine:** Unreal Engine 5.7
* **Programming Language:** C++ & Blueprints
* **Backend / Auth:** Google Firebase Authentication (REST API)
* **Networking:** Unreal Engine OnlineSubsystem (Null/LAN)

##  주요 기능 (Key Features)

### 1.  사용자 인증 시스템 (Firebase Auth)
* 언리얼 엔진 C++의 `Http` 모듈을 활용하여 Firebase REST API와 통신합니다.
* **이메일 회원가입 및 로그인:** 사용자는 이메일과 닉네임을 입력하여 계정을 생성하고 로그인할 수 있습니다.
* **게스트 로그인:** 계정이 없는 사용자를 위해 임시 랜덤 닉네임을 부여하는 게스트 접속 기능을 지원합니다.
* **보안:** `UGameInstance` 클래스에서 토큰(Token)과 UID를 안전하게 관리하여 게임 내내 세션을 유지합니다.

### 2.  멀티플레이어 로비 시스템 (Lobby & Session)
* **방 생성 (Host):** 사용자가 방 제목을 입력하여 새로운 게임 세션을 호스팅할 수 있습니다.
* **방 목록 조회 및 참가 (Find & Join):** 현재 활성화된 방 목록(방장 닉네임, 방 제목, 참가 인원)을 실시간으로 검색하고 참가할 수 있습니다.
* **대기방 시스템 (Waiting Room):** 게임 시작 전 방장과 참가자들이 모여 대기할 수 있는 공간이 마련되어 있습니다.

### 3.  견고한 네트워크 예외 처리
* 언리얼 엔진의 `OnNetworkFailure` 델리게이트를 C++ 단에서 오버라이드하여 예외 상황을 완벽하게 처리합니다.
* 방장이 방을 폭파(퇴장)할 경우, 참가자들의 화면에 "방장이 접속을 끊었습니다"라는 알림 팝업과 함께 안전하게 기본 로비 맵으로 복귀시킵니다.
* 엔진 내부의 `GameSession` 찌꺼기를 확실하게 파기(`DestroySession`)하여 즉시 다른 방에 재접속이 가능하도록 구현했습니다.

##  맵 구성 (Map Flow)
1. **TitleMap:** 게임 최초 실행 시 진입. 로그인, 회원가입, 게스트 모드 UI 제공.
2. **LobbyMap:** 로그인 성공 시 진입. 실시간 방 목록 확인, 방 생성 및 참가 가능.
3. **WaitingRoomMap:** 방 접속 성공 시 진입. 멀티플레이어 대기실.

##  실행 방법 (How to Run)
1. 프로젝트를 `.zip`으로 다운로드 받거나 `git clone` 합니다.
2. `BombTag.uproject`를 우클릭하고 **Generate Visual Studio project files**를 실행합니다.
3. `BombTag.sln`을 열고 **Development Editor** 구성으로 빌드합니다.
4. 언리얼 에디터에서 프로젝트를 열고 `TitleMap`을 실행하여 게임을 플레이합니다.
* **멀티플레이 테스트:** 패키징된 `.exe` 파일을 2개 실행하거나, 에디터의 플레이 모드를 `Standalone Game` 및 `Net Mode: Play As Listen Server`로 설정하여 테스트할 수 있습니다. (동일 LAN 환경 필요)

---