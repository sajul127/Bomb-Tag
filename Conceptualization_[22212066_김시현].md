#1. Conceptualization

**Project Title**:Bomb-Tag
<br/><br/>
![Image](https://github.com/user-attachments/assets/b9068037-94cd-4ddd-8ab4-ee5f91be054e)

**Student Info**: (22212066, 김시현, rlatlgus127@gmail.com)

### [ Revision history ]

| Revision date | Version # | Description | Author |
| --- | --- | --- | --- |
| 03/27/2026 | 0.01 | Plan | 김시현 |

---

### = Contents =
1. Business purpose
2. System context diagram
3. Use case list
4. Concept of operation
5. Problem statement
6. Glossary
7. References

---

## 1. Business purpose

**1) Project background**
* 현대인들에게 게임은 가장 접근하기 쉬운 스트레스 해소 수단이자 필수적인 오락거리로 자리 잡았다.
* 하지만 최근 출시되는 주류 게임(MMORPG, 고도화된 전략 게임 등)은 방대한 세계관과 복잡한 시스템을 갖추고 있어, 유저들에게 긴 학습 시간과 무거운 플레이 타임을 요구한다. 
* 이러한 하드코어 게임들은 바쁜 일상 속에서 짧게 여가를 즐기려는 사람들에게 오히려 진입 장벽과 피로감을 유발하기도 한다. 
* 이러한 흐름의 반동으로, 최근 게임 시장에서는 튜토리얼 없이도 직관적으로 이해할 수 있고 짧은 시간 안에 강렬한 재미를 주는 '하이퍼 캐주얼(Hyper-casual) 게임'과 '파티 게임(Party Game)' 장르가 큰 인기를 끌고 있다. 
* 오프라인에서 흔히 하던 '폭탄 돌리기(혹은 수건돌리기)'는 남녀노소 누구나 아는 단순한 규칙을 가지고 있다. 
* 하지만 '제한 시간이 끝나기 전에 타인에게 위험을 떠넘겨야 한다'는 원칙은 플레이어에게 극도의 몰입감과 긴장감을 선사한다. 
* 만약 이 원초적인 재미를 온라인 실시간 멀티플레이 환경으로 완벽하게 이식한다면, 지인들과의 가벼운 내기(예: 밥값 내기, 심부름 정하기)나 아이스브레이킹이 필요한 순간에 언제 어디서든 활용할 수 있는 훌륭한 매개체가 될 것이다. 
* 따라서 본 프로젝트는 화려한 그래픽이나 복잡한 성장 요소 대신, '직관적인 규칙'과 플레이어 간의 '실시간 추격 및 충돌'이라는 게임의 본질적인 재미에 집중한 온라인 멀티플레이 미니게임을 개발하고자 한다. 

**2) Goal**
* **실시간 네트워크 환경 구축**: 플레이어들이 룸(Room)을 생성하고 접속하여, 서로의 이동 위치와 상태(폭탄 소유 여부)가 지연 없이 동기화되는 멀티플레이 환경을 구현한다.
* **직관적이고 빠른 템포의 게임성 제공**: 별도의 학습 없이 접속 즉시 플레이가 가능하도록 룰을 직관화하고, 3~5분 내외의 짧은 플레이 타임으로 속도감 있는 게임 경험을 제공한다.
* **공정하고 정확한 판정 시스템**: 플레이어 간의 충돌(폭탄 양도) 및 제한 시간 만료 시점의 판정을 서버 기준으로 정확하게 처리하여 승패의 신뢰도를 높인다.

**3) Target Market**
* 복잡한 시스템이나 긴 플레이 타임에 지쳐, 가볍고 빠르게 즐길 수 있는 스낵 컬처(Snack Culture)형 게임을 선호하는 유저.
* 학교, 직장, 모임 등에서 친구 혹은 동료들과 간단한 내기나 벌칙을 정하기 위해 공정하고 재미있는 수단을 찾는 그룹.
* 모바일이나 PC 환경에서 핑(Ping) 차이 없이 원활한 실시간 상호작용을 경험하고 싶은 캐주얼 게이머.

---

## 2. System context diagram
<br/><br/>
![Image](https://github.com/user-attachments/assets/c21b94da-0ec0-4bd1-ad1f-3c31b1221327)

```text
Login: 로그인
Set Name: 닉네임(이름) 설정
Create Room: 방 만들기 (호스트)
Join Room: 방 참여 (클라이언트)
Move Character: 캐릭터 이동 (플레이어 조작)
Transfer Bomb: 폭탄 양도 (상호작용 키 입력)
Leave Room: 방 떠나기 (로비로 이동 또는 호스트 위임)
Display Game State: 게임 상태 화면 출력 (플레이어 화면)
Show Timer: 남은 시간 표시
Show Result: 게임 결과(승/패 및 생존자) 표시
Request Login / Create / Join Room: 서버에 로그인 및 방 생성/참여 요청
Send Position & Bomb Data: 플레이어의 이동 좌표 및 폭탄 소유 상태를 서버로 전송
Confirm Login, Room Info: 서버에서 로그인 승인 및 방 정보 할당
Sync Position & Bomb Status: 서버에서 모든 클라이언트에게 플레이어 위치 및 폭탄 상태 동기화
Broadcast Time & Result: 서버에서 전체 유저에게 타이머 동기화 및 라운드 결과 브로드캐스트
```
## 3. Use case list

**1) 닉네임 설정 및 로그인**

| Actor | Player|
| --- | --- |
| Description | 게임에 접속한 유저가 자신을 식별할 수 있는 닉네임을 설정하고 로비 서버에 접속한다. |

**2) 방 만들기 (Create Room)**

| Actor | Player (Host) |
| --- | --- |
| Description | 유저가 새로운 게임 세션을 생성한다. 방을 만든 유저는 리슨 서버(Listen Server)의 호스트가 되어 게임 룰과 통신을 주관한다. |

**3) 방 참여 (Join Room)**

| Actor | Player (Client) |
| --- | --- |
| Description | 유저가 생성되어 있는 게임 세션(방) 목록을 확인하고, 원하는 방에 클라이언트로 접속한다. |

**4) 캐릭터 이동**

| Actor| Player, System |
| --- | --- |
| Description | 유저가 조이스틱이나 키보드를 조작하여 캐릭터를 이동시킨다. 시스템은 이 이동 좌표를 서버에 전달하고 다른 모든 유저의 화면에 동기화한다. |

**5) 폭탄 양도 (상호작용 키 입력)**

| Actor | Player, System, Server |
| --- | --- |
| Description | 폭탄을 소유한 플레이어가 다른 플레이어의 근처(일정 반경 내)에 접근한 상태에서 상호작용 키(예: 스페이스바)를 누르면, 서버에 양도를 요청하여 폭탄 소유권을 넘긴다. |

**6) 게임 결과 확인**

| Actor | Player, Server |
| --- | --- |
| Description | 제한 시간이 종료되어 폭탄이 터지면, 서버가 탈락자와 생존자를 판정하여 모든 플레이어의 화면에 게임 결과를 출력한다. |

**7) 방 떠나기**

| Actor | Player |
| --- | --- |
| Description | 게임 전후에 유저가 현재 속한 세션을 떠나 메인 로비로 돌아간다. 호스트가 떠날 경우 서버 세션이 닫히거나 다른 유저에게 권한이 위임된다. |

---

## 4. Concept of operation

**1) 닉네임 설정 및 로그인**

| Purpose | 유저 식별 및 멀티플레이 세션 진입 준비. |
| --- | --- |
| Approach | UI에서 닉네임을 입력받아 고유 ID와 매칭 후 저장. |
| Dynamics | 앱 최초 실행 및 멀티플레이 로비 접속 시. |
| Goals | 혼선 없는 고유 플레이어 프로필 생성. |

**2) 방 만들기 (Host Session)**

| Purpose | 새로운 멀티플레이 네트워크 게임 세션 생성. |
| --- | --- |
| Approach | 리슨 서버(Listen Server) 형태로 방 개설 후 호스트 권한 부여. |
| Dynamics | 유저가 '방 만들기' 버튼 클릭 시. |
| Goals | 타 유저가 접속 가능한 안정적인 게임 세션 구축. |

**3) 방 참여 (Join Session)**

| Purpose | 생성된 방 검색 및 클라이언트로 접속. |
| --- | --- |
| Approach | 방 목록 UI에서 특정 방 선택 후 호스트 IP로 연결(ClientTravel) 시도. |
| Dynamics | 다른 유저가 만든 방에 합류하고자 할 때. |
| Goals | 방장과 클라이언트 간의 네트워크 연결 및 권한 할당. |

**4) 캐릭터 이동 동기화**

| Purpose | 맵 내 유저들의 실시간 위치 동기화. |
| --- | --- |
| Approach | 서버로 이동 입력 전송 및 클라이언트 예측/보간(Interpolation) 기능 활용. |
| Dynamics | 유저가 캐릭터를 이동 조작할 때. |
| Goals | 네트워크 지연(Ping) 환경에서도 자연스럽고 부드러운 이동 제공. |

**5) 폭탄 양도 및 판정**

| Purpose | 유저 조작(상호작용 키)을 통한 공정한 폭탄 양도 처리. |
| --- | --- |
| Approach | 타겟 접근 후 스페이스바 입력 시, 서버 권한(Server Authority)으로 거리 검증 및 소유권 변경. |
| Dynamics | 폭탄 소유자가 타겟 근처에서 스페이스바 입력 시. |
| Goals | 서버 기준의 철저한 검증으로 억울함 없는 공정한 양도 구현. |

**6) 타이머 및 결과 동기화**

| Purpose | 제한 시간 공유 및 종료 시 결과 출력. |
| --- | --- |
| Approach | 서버(GameMode)가 타이머를 관리하고, 0초 도달 시 탈락자 판정 및 결과 UI 브로드캐스트. |
| Dynamics | 라운드 제한 시간이 0초가 되어 폭탄이 터질 때. |
| Goals | 모든 유저에게 오차 없는 동일한 시간 및 승패 결과 제공. |

**7) 방 떠나기 (Destroy Session)**

| Purpos | 세션 안전 파기 및 메인 로비 복귀. |
| --- | --- |
| Approach | 세션 연결 해제(Destroy Session) 후 로비로 이동. (방장 이탈 시 방 폭파 처리) |
| Dynamics | 유저가 자의로 나가거나 게임 라운드가 완전히 종료된 후. |
| Goals | 세션 데이터를 깔끔하게 초기화하여 다음 접속 시 오류 방지. |

---

## 5. Problem statement

**Overview**<br/>
'Bomb-Tag(한글 버전 : 나만 아니면 돼~)'는 언리얼 엔진의 멀티플레이 프레임워크를 기반으로 플레이어 간 게임 환경을 실시간으로 동기화한다.
빠른 템포의 추격전과 충돌 판정이 핵심 재미 요소이므로, 네트워크 지연(Lag) 처리와 서버 아키텍처 관리가 가장 중요한 기술적 과제이다.

**Problem #1: 서버 호스팅 아키텍처 (Listen Server 도입)**
* 언리얼 엔진은 데디케이티드 서버(서버 전용 머신)와 리슨 서버(플레이어 중 한 명이 호스트 겸 서버 역할) 방식을 모두 지원한다. 
* 본 프로젝트는 별도의 서버 유지 비용 없이 유저들끼리 쉽게 게임을 즐길 수 있도록 리슨 서버(Listen Server) 방식을 채택한다. 
* 단, 호스트 유저는 통신 지연이 0(Zero)이라는 이점을 가지므로, 판정 시 호스트와 클라이언트 간의 불공정을 완화할 수 있는 물리 보정 수치를 고려해야 한다.

**Problem #2: 네트워크 지연(Ping)과 데이터 무결성**
* 클라이언트 간 통신 지연으로 인해 내 화면에서는 상대에게 닿았지만, 서버에서는 아직 닿지 않은 것으로 판정되는 핑 문제가 필연적으로 발생한다. 
* 이동은 유저의 답답함을 없애기 위해 클라이언트 예측(Client Prediction)을 허용하되, 폭탄이 터지는 판정이나 타인에게 양도되는 판정은 철저히 **서버 권한(Server Authority)**으로만 처리하여 데이터의 무결성을 보장해야 한다.

**Problem #3: 빌드 용량 및 리소스 최적화**
* 언리얼 엔진의 특성상 캐주얼 미니게임임에도 초기 빌드 용량이 과도하게 커질 수 있다. 
* 불필요한 기본 플러그인을 모두 비활성화하고, 텍스처 해상도 및 에셋의 압축 세팅을 프로젝트 스케일에 맞게 조절하여 쾌적한 다운로드 및 실행 환경을 제공해야 한다.

**NFRS (비기능적 요구사항)**
* a. 게임 엔진은 Unreal Engine 5.7.1 버전을 사용한다.
* b. 개발 언어는 **C++ 및 블루프린트(Blueprint)**를 프로젝트 아키텍처에 맞게 혼용한다.
* c. 네트워크 통신은 언리얼 엔진의 내장 Replication 시스템과 Online Subsystem(Null 또는 Steam)을 사용한다.
* d. 동기화에 사용되는 중요 로직(타이머, 충돌 판정)은 클라이언트가 아닌 서버 프레임워크(GameMode)에 종속시킨다.

---

## 6. Glossary

| 용어 | 설명 |
| --- | --- |
| Unreal Engine | 고품질 그래픽과 강력한 자체 네트워크 프레임워크를 기본으로 제공하는 글로벌 게임 개발 엔진. |
| Replication (리플리케이션) | 서버의 액터(Actor)나 변수 데이터를 클라이언트들에게 자동으로 동기화해 주는 언리얼 엔진의 핵심 네트워크 시스템. |
| RPC (Remote Procedure Call) | 클라이언트가 서버의 특정 함수를 실행하도록 요청하거나(Server RPC), 서버가 모든 클라이언트의 함수를 실행하게 하는(Multicast RPC) 원격 제어 통신 기법. |
| Listen Server (리슨 서버) | 별도의 물리적 서버를 두지 않고, 플레이어 중 한 명이 방장(Host)이자 서버 역할을 동시에 수행하는 멀티플레이 아키텍처. |
| GameMode / GameState | 게임의 핵심 규칙 및 권한(GameMode, 서버에만 존재)과 현재 진행 상태 및 데이터(GameState, 모두에게 공유됨)를 관리하는 언리얼 엔진의 뼈대 클래스. |
| Capsule Component | 캐릭터의 충돌을 감지하기 위해 캐릭터 외부를 감싸고 있는 캡슐 형태의 물리 충돌체(Collider). |

---

## 7. References

1. **Unreal Engine 5 Documentation**: [언리얼 엔진 5 문서 가이드 (Epic Developer Community)](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/unreal-engine-5-7-documentation)
2. **Epic Developer Community**: [Online Subsystem & Replication 개발자 커뮤니티 문서](https://dev.epicgames.com/community/)
3. **Unreal Engine Multiplayer Networking Tutorial**: [언리얼 엔진 네트워크 및 멀티플레이어 공식 가이드](https://dev.epicgames.com/documentation/en-us/unreal-engine/networking-and-multiplayer-in-unreal-engine?application_version=5.0)
4. **Google Gemini 1.5 Pro**: [구글 제미나이](https://gemini.google.com/app?hl=ko)