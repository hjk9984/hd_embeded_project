# hd_embeded_project

## status
int 타입으로 설정
```c
// It will be scripted in Status.h
#define DOOR_LOCK 1         // LED RED, RGB RED
#define DOOR_UNLOCK 2       // LED BLUE, RGB RED
#define CAR_IN_UNLOCK 3     // LED BLUE, RGB BLUE
#define CAR_IN_LOCK 4       // LED BLUE, RGB RED
```
구현한 함수는 차고의 상태를 확인하고 넣는다.
```c
void foo (int status, ...){
  if(status != DOOR_UNLOCK)
    return -1;
  
  ...
  
}

```

## commit message rule
* Init {file name} : 초기화
* Add {file name} : 파일을 추가할 떄
* Update {file name} {content} : 이미 있는 파일에 어떤 내용(함수 등등)을 보완할 때
* Remove {file name} : 파일 삭제

기타 등등
* Fix type : 오타 수정

참고
https://haesoo9410.tistory.com/299?category=909362
