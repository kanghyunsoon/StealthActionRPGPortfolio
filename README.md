# Stealth Action RPG

잠입, 커버 이동, 근접·원거리 전투, 보스전을 하나의 플레이 흐름으로 만든 1인 개발 프로젝트입니다. Unreal Engine 4.27의 C++과 Blueprint로 구현했습니다.

<p>
  <img src="https://img.shields.io/badge/Unreal_Engine_4.27-0E1128?style=flat-square&amp;logo=unrealengine&amp;logoColor=white" alt="Unreal Engine 4.27" />
  <img src="https://img.shields.io/badge/C%2B%2B-00599C?style=flat-square&amp;logo=cplusplus&amp;logoColor=white" alt="C++" />
  <img src="https://img.shields.io/badge/Blueprint-137CBD?style=flat-square&amp;logo=unrealengine&amp;logoColor=white" alt="Blueprint" />
  <img src="https://img.shields.io/badge/Behavior_Tree-1F2937?style=flat-square" alt="Behavior Tree" />
</p>

[전체 플레이 영상](https://youtu.be/WUUj8kRv9-s)

![탐험 지역과 성을 바라보는 플레이 화면](docs/assets/portfolio/world.jpg)

## 프로젝트 요약

| 구분 | 내용 |
| --- | --- |
| 개발 | 1인 개발 |
| 공개 | 2024.05 |
| 담당 | 기획, 플레이어 제어, 전투, 커버, AI, UI 연결 |
| 결과물 | 탐색·잠입·전투·보스전을 포함한 플레이 데모 |

첫 Unreal 프로젝트에서 하나의 기능을 만드는 것에서 끝내지 않고, 탐색부터 일반 적과 보스 전투까지 연결했습니다. 이 저장소에는 제가 설명할 수 있는 C++ 코드만 선별해 담았습니다.

![커버, Feet IK, AI, 지면 설정 수치](docs/assets/portfolio/metrics.svg)

수치는 공개 코드의 트레이스 구조, AI 상태 enum, 프로젝트 설정을 기준으로 적었습니다.

## 플레이 화면

<table>
  <tr>
    <td width="50%"><img src="docs/assets/portfolio/inventory.jpg" alt="아이템 습득과 인벤토리" /></td>
    <td width="50%"><img src="docs/assets/portfolio/combat.jpg" alt="필드 근접 전투" /></td>
  </tr>
  <tr>
    <td width="50%"><img src="docs/assets/portfolio/stealth.jpg" alt="건물 주변 잠입 플레이" /></td>
    <td width="50%"><img src="docs/assets/portfolio/cover.jpg" alt="벽을 이용한 커버 플레이" /></td>
  </tr>
  <tr>
    <td width="50%"><img src="docs/assets/portfolio/boss.jpg" alt="보스와 소환체 전투" /></td>
    <td width="50%"><img src="docs/assets/portfolio/ranged.jpg" alt="활을 사용한 원거리 전투" /></td>
  </tr>
</table>

![보스 광역 공격을 회피하는 화면](docs/assets/portfolio/boss-aoe.jpg)

## 문제 해결

### 1. 커버 진입 판정을 3지점으로 바꾸었다

**문제.** 전방 한 지점만 확인하면 캐릭터 양쪽이 벽에 붙을 수 있는지 판단할 수 없었습니다. 벽 끝에서 커버에 진입하거나 코너를 돌 때 캐릭터 방향이 틀어졌습니다.

**해결.** 캐릭터 중앙·좌·우에서 Sphere Sweep를 수행하고, 세 지점이 모두 벽을 탐지한 경우만 커버 진입을 허용했습니다. 진입 후에는 히트 법선으로 위치와 회전을 맞추고, 좌우 탐지 결과로 벽면 이동과 커버 끝 카메라를 처리했습니다.

**결과.** 커버 진입, 벽면 이동, 코너 회전, 커버 종료를 하나의 상태로 연결했습니다.

### 2. 지면 탐지 결과를 Feet IK와 발소리에 같이 사용했다

**문제.** 계단과 경사면에서 발이 뜨고, 지면이 바뀌어도 같은 발소리가 났습니다.

**해결.** 양발 소켓에서 바닥으로 라인 트레이스를 수행했습니다. 히트 거리로 발과 골반 오프셋을 계산하고, 바닥 법선으로 발의 Pitch와 Roll을 보정했습니다. Anim Notify에서는 Physical Material을 조회해 지면별 사운드를 `TMap`에서 선택했습니다.

**결과.** 애니메이션 보정값과 발소리를 지면 탐지 결과로 연결했습니다. 파쿠르 동작 중에는 Feet IK를 끄도록 분기했습니다.

### 3. AI 판단과 공격 실행을 나눠 추적했다

**문제.** 타깃, 거리, 피격 상태와 공격 실행을 한 곳에서 처리하면 적이 왜 해당 행동을 했는지 확인하기 어려웠습니다.

**해결.** Behavior Tree Service는 타깃·거리·피격 여부를 판단하고, Blackboard는 7개 상태를 보관하며, Task는 이동과 공격을 실행하도록 나뀌었습니다. 보스 광역 공격은 데칼 예고, 지연, 범위 판정, Anim Notify 순으로 구성했습니다.

**결과.** 판단 조건과 실행 시점을 따로 확인할 수 있게 됐고, 보스 공격은 예고를 보고 회피할 수 있는 패턴이 됐습니다.

## 코드에서 확인할 부분

- [커버 탐지·정렬·이동·카메라](Source/Portpolio/Components/CCoverSystemComponent.cpp)
- [양발 트레이스와 Feet IK 데이터](Source/Portpolio/Components/CFeetComponent.cpp)
- [Animation Instance의 IK 적용 조건](Source/Portpolio/Characters/CAnimInstance.cpp)
- [Physical Material 기반 발소리](Source/Portpolio/FootStep/CAnimNotify_FootStepSound.cpp)
- [Blackboard 기반 AI 상태](Source/Portpolio/Behavior/CAIBehaviorComponent.cpp)
- [거리·피격·타깃 상태 판단](Source/Portpolio/Behavior/Services/BTService_Range.cpp)
- [BT Action 실행·종료·취소](Source/Portpolio/Behavior/Tasks/BTTaskNode_Action.cpp)

## 첫 프로젝트에서 남은 한계

- 커버 컴포넌트에 탐지, 이동, 카메라, 애니메이션 상태가 모였습니다. 다시 만든다면 탐지·이동·카메라 책임을 나눕니다.
- 검증은 Unreal Editor에서 장면별로 반복 플레이하는 방식이었고, 자동화 테스트는 없었습니다.
- 실행에 필요한 Blueprint과 상용·외부 에셋을 제외했기 때문에 이 공개본만으로는 프로젝트를 실행할 수 없습니다.

> 이 저장소는 직접 구현한 C++ 코드와 실제 플레이 영상에서 추출한 화면만 담았습니다. 맵, Blueprint, 모델, 애니메이션, 사운드, Marketplace 에셋, 플러그인, 빌드 산출물은 제외했습니다. 공개 범위는 [NOTICE](NOTICE.md)에 적었습니다.
