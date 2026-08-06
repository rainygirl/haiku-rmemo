# R Memo for Haiku OS

Haiku OS 에서 작동하는 데스크톱에 붙여두는 메모입니다. 메모처럼 생겼습니다.

![Haiku 데스크톱에 붙은 메모 세 개](screenshots/notes.png)

메모 하나가 테두리 없는 창이고, 창이 스스로 자기 모습을 그립니다 — 색지,
위쪽 헤더 띠, 오른쪽 아래 접힌 모서리. 창 장식(데코레이터)을 전혀 쓰지
않는데, 그게 핵심입니다. 제목 표시줄이 달린 메모는 메모가 아니라 대화상자로
보입니다.

## 동작

- **헤더가 이동 손잡이입니다.** 아무 데나 잡고 끌면 됩니다.
- **접힌 모서리가 크기 조절 손잡이입니다.** 당기면 크기가 바뀝니다.
- **왼쪽 동그라미를 누르면 색상 메뉴**가 나옵니다 (7색).
- **`+` 는 새 메모**를 만듭니다. 현재 메모의 오른쪽 아래로 조금씩 어긋나게
  놓입니다.
- **`×` 는 메모를 버립니다.** 내용이 적혀 있으면 먼저 물어봅니다.
- **마지막 메모를 닫으면 앱이 종료**되고, 저장된 메모가 없는 상태로 실행하면
  빈 메모 하나가 열립니다. 창 없이 떠 있는 상태도, 돌아갈 방법 없는 창도
  생기지 않습니다.

메모는 종료할 때 `~/config/settings/RMemo/notes` 에 저장되고, 다음
실행에서 내용·위치·크기·색상이 그대로 복원됩니다.

## 요구 사항

Haiku OS (x86 또는 x86_64). 실행할 기기에서 그대로 빌드하시면 됩니다.

## 빌드 및 설치

```sh
./install.sh
./install.sh --build-only   # 빌드만 하고 설치하지 않음
./install.sh --uninstall    # 제거
```

`install.sh` 는 바이너리를 `~/config/non-packaged/apps/` 에 넣고
`~/config/non-packaged/data/deskbar/menu/Applications/` 에 심볼릭 링크를
만듭니다. 이 링크가 있어야 **Deskbar → Applications** 에 나타납니다.
Deskbar 는 `~/config/settings/deskbar/menu` 를 직접 읽지 않고
`/boot/system/data/deskbar/menu_entries` 에 적힌 검색 경로 목록을 따르는데,
그 목록에서 사용자가 쓸 수 있는 항목이 위의 `non-packaged` 경로입니다.

## 라이선스

MIT

## AI 기여 고지

이 프로그램은 Claude와 함께 작업해 만들었습니다.
