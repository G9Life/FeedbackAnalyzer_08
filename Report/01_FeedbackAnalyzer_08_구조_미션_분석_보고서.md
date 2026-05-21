# FeedbackAnalyzer_08 — 전체 구조·미션 안내 분석 보고서

| 항목 | 내용 |
|------|------|
| **프로젝트명** | 리팩토링 챌린지: 고객 피드백 분석 시스템 (Feedback Analyzer) |
| **작성일** | 2026-05-21 |
| **분석 대상** | `c:\DEV\FeedbackAnalyzer_08` |
| **참고 문서** | `README.md`, `project_purpose.md`, `src/cpp/*` |

---

## 1. 요약 (Executive Summary)

FeedbackAnalyzer_08은 **C++17 + cpp-httplib** 기반의 고객 피드백 분석 웹 애플리케이션입니다. 텍스트·CSV 입력, 키워드·감정 분류, 필터링, 결과 시각화·CSV 다운로드를 제공합니다.

본 프로젝트는 **운영용 제품이 아니라 리팩토링 학습용 코드베이스**입니다. `project_purpose.md`에 명시된 대로 긴 함수, 중복 로직, 전역 상태, God Object 등 **의도적 코드 스멜**이 포함되어 있으며, 학습자는 8단계 미션(총 약 13시간)을 통해 테스트·버그 수정·구조 개선·기능 확장을 수행합니다.

**핵심 결론**

- 실행 진입점·UI·라우팅이 `main.cpp`에 집중되어 유지보수 부담이 큼
- `TextAnalyzer`와 `Filters`의 감정 판별·키워드 소스가 분리되어 **필터 오류(특히 "중립")** 가능성이 높음
- 테스트·Trend CSV·File DB는 미션 후반 단계에서 **신규 구축**이 필요함

---

## 2. 프로젝트 목적 및 학습 목표

### 2.1 프로젝트 목적

자연어 기반 고객 피드백을 수집·분류·시각화하는 웹 앱을 제공하되, 초기 코드는 구조화되지 않은 상태로 제공됩니다. 학습자는 코드 스멜을 식별하고 클린 아키텍처 방향으로 리팩토링합니다.

### 2.2 주요 학습 목표

| # | 학습 목표 |
|---|-----------|
| 1 | 코드 스멜 및 안티패턴 식별 |
| 2 | 모듈화된 아키텍처로 구조 개선 |
| 3 | 비즈니스 로직과 UI(HTML 렌더링) 분리 |
| 4 | Extract Function/Class, 전략 패턴 등 리팩토링 기법 실습 |
| 5 | C++ 웹 애플리케이션 유지보수성·테스트 가능성 향상 |

### 2.3 대상 학습자

- 중급 이상 C++ 개발자
- TDD, Clean Code, Refactoring 실습이 필요한 개발자

---

## 3. 기술 스택 및 빌드·실행

### 3.1 기술 스택

| 구분 | 기술 |
|------|------|
| 언어 | C++17 |
| 빌드 | CMake 3.14+ |
| HTTP | cpp-httplib (`src/cpp/httplib.h`) |
| 플랫폼 | Windows(MinGW/MSVC), `ws2_32` 링크 |

### 3.2 빌드 및 실행

```text
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe
cmake --build build
build\feedback_analyzer.exe
```

- 접속 URL: `http://localhost:8080`
- 리스닝: `0.0.0.0:8080`

### 3.3 입력 CSV 형식

- 필수 컬럼: `text`
- 첫 행은 헤더로 간주하고 데이터 파싱 시 스킵

---

## 4. 디렉터리 및 파일 구조

```text
FeedbackAnalyzer_08/
├── CMakeLists.txt              # 단일 타깃 feedback_analyzer
├── README.md                   # 사용법·빌드 안내
├── project_purpose.md          # 목적·스멜 목록·8단계 미션
├── Report/                     # 본 보고서
│   └── FeedbackAnalyzer_08_구조_미션_분석_보고서.md
└── src/cpp/
    ├── main.cpp                # HTTP 서버, 라우팅, HTML 렌더링
    ├── httplib.h               # 서드파티 HTTP 라이브러리
    ├── Feedback.h              # 피드백 데이터 모델
    ├── TextAnalyzer.h/cpp      # 감정·키워드 집계
    ├── Filters.h/cpp           # 필터링
    ├── Constants.h/cpp           # 감정·카테고리 키워드
    ├── Session.h/cpp           # 세션(전역) 상태
    ├── Logger.h/cpp            # 로깅
    ├── UIComponents.h/cpp      # UI 카테고리 목록
    └── FileHandler.h           # 파일 처리(미연동, 죽은 코드)
```

### 4.1 CMake 빌드 대상

`CMakeLists.txt`는 다음 소스만 컴파일합니다.

- `main.cpp`, `Constants.cpp`, `Filters.cpp`, `Logger.cpp`, `Session.cpp`, `TextAnalyzer.cpp`, `UIComponents.cpp`

`FileHandler`는 헤더만 존재하며 별도 `.cpp` 없음. `main.cpp`에서 인스턴스 생성만 하고 호출하지 않음.

---

## 5. 기능 명세 (리팩토링 전 기준)

### 5.1 핵심 기능

| 기능 | 설명 |
|------|------|
| 대시보드 | 웹 UI 제공 |
| 텍스트 입력 | 단건 피드백 POST |
| CSV 업로드 | multipart 파일 업로드 |
| 감정 분석 | 긍정 / 중립 / 부정 (키워드 매칭) |
| 키워드 분류 | 배송, 품질, 가격, 서비스, 사용성 |
| 필터링 | 감정·키워드 조합 필터 |
| 시각화 | 감정·키워드 분포 통계 카드 |
| 다운로드 | 필터 결과 CSV (`GET /download`) |

### 5.2 사용자 시나리오

1. 브라우저에서 `http://localhost:8080` 접속
2. 피드백 텍스트 입력 또는 CSV 업로드
3. 시스템이 키워드·감정 기반 분류·집계
4. 감정·키워드 필터로 결과 좁히기
5. 통계 확인 후 필요 시 CSV 다운로드

### 5.3 입·출력 예시

| 기능 | 입력 예시 | 출력 예시 |
|------|-----------|-----------|
| 텍스트 입력 | "배송이 너무 늦어요. 화가 납니다." | 감정: 부정, 카테고리: 배송 |
| CSV 업로드 | `feedbacks.csv` (text 컬럼) | 전체 분석 테이블·그래프 |
| 감정 분석 | — | 긍정 40%, 중립 20%, 부정 40% |
| 키워드 필터 | "배송" | 배송 관련 피드백 N건 |

---

## 6. 런타임 아키텍처

### 6.1 HTTP 라우트

| 메서드 | 경로 | 처리 내용 |
|--------|------|-----------|
| GET | `/` | 세션 초기화 호출, 시작 메시지와 빈 대시보드 |
| POST | `/analyze` | 폼 `text` 파싱 → `Session::currentFeedbacks` 추가 → `sent()`, `kw()` |
| POST | `/upload` | CSV 파싱(첫 줄 헤더 스킵) → 피드백 누적 |
| POST | `/filter` | `Filters::fil()` → 통계 재계산, `fil_data` 갱신 |
| GET | `/download` | `fil_data` 기준 UTF-8 BOM CSV 첨부 |

### 6.2 요청 처리 흐름 (개념)

```text
[Browser]
    │
    ▼
[main.cpp — httplib::Server]
    │
    ├── Session::getCurrentFeedbacks()  ← 피드백 저장소
    ├── TextAnalyzer::sent() / kw()   ← 집계 (Constants 참조)
    ├── Filters::fil()                ← 필터 (S_KEYWORDS + Constants)
    ├── Logger::log*()                ← 콘솔 출력
    └── renderPage()                  ← HTML 생성·응답
```

### 6.3 상태 저장 위치

| 상태 | 위치 | 용도 |
|------|------|------|
| 전체 피드백 | `Session::currentFeedbacks` (static) | 입력·업로드 누적 |
| 필터 결과 | `main.cpp` — `fil_data` (static) | 다운로드·필터 후 표시 |
| 분석 캐시 | `TextAnalyzer::globalSent`, `globalKw` | 전역 map (사용처 제한적) |
| 필터 감정 키워드 | `Filters::S_KEYWORDS` | Constants와 **별도** 초기화 |

---

## 7. 모듈별 상세 분석

### 7.1 main.cpp

**역할**: 서버 기동, 라우트 등록, 폼/CSV 파싱, HTML 렌더링, 예외 처리.

**주요 정적 함수**

- `urlDecode`, `parseForm`, `escapeHtml`, `renderPage`, `parseCsvLine`

**특징·이슈**

- `renderPage()`에 CSS·폼·통계 UI가 모두 포함 (200줄 이상) → **God Function**
- 비즈니스 로직과 프레젠테이션 미분리
- `fileHandler` 인스턴스 생성 후 미사용

### 7.2 Feedback.h

```cpp
class Feedback {
    std::string text;
public:
    explicit Feedback(std::string t);
    const std::string& getText() const;
};
```

- setter 없음, 분석 결과(감정·카테고리) 필드 없음 → **데이터 클래스 미완성** (미션 5.2 개선 대상)

### 7.3 TextAnalyzer

| 메서드 | 동작 |
|--------|------|
| `sent()` | 긍정 키워드 매칭 → 부정 → 그 외 **중립** (중립 키워드 목록 미사용) |
| `kw()` | `Constants::CATEGORY_KEYWORDS`의 `"main"` 서브키만 카운트 |

- `containsAny()` — Filters와 **중복 구현**
- `globalSent`, `globalKw` — static 전역 캐시

### 7.4 Filters

| 메서드 | 동작 |
|--------|------|
| `initFilterKeywords()` | `S_KEYWORDS`에 긍정/부정/중립 키워드 **별도** 등록 |
| `fil()` | 감정 필터 후 키워드 필터; 결과를 `cout`으로 출력 |

**알려진 불일치·버그 후보 (미션 3단계)**

1. **감정 판별 불일치**: `TextAnalyzer`는 중립 키워드를 보지 않음. `Filters`는 긍정→부정→중립 키워드 순으로 판별. 동일 문장이 통계와 필터에서 다르게 분류될 수 있음 (예: "괜찮"이 긍정·중립 양쪽 키워드에 존재).
2. **키워드 필터**: `kFilter != "전체"`일 때 `subEntry.first == "main"`이면 **continue** → `"main"`에만 있는 키워드(예: "배송")로는 필터 매칭 실패 가능.

### 7.5 Constants

- `SENTIMENT_KEYWORDS`, `CATEGORY_KEYWORDS` map 하드코딩
- 감정 키워드 **중복 등록** (동일 벡터 반복 push)
- 카테고리: 배송, 품질, 가격, 서비스, 사용성 — 각각 `main` 및 서브 키(`time`, `type` 등)

### 7.6 Session

- `currentFeedbacks`: 실제 사용되는 유일한 유의미 상태
- `internalData`, `filterOptions`: 정의만 있고 **미사용**
- `getOldDataFromSession(key)`: `key` 무시 → API 혼란
- `initSessionStateUgly()`: 빈 구현 (의도적 스멜)

### 7.7 Logger

- `logInfo` / `logWarning` → `stdout`, `logError` → `stderr`
- `debugMode` 기본 `true` (`Logger.cpp`)
- **웹 페이지 alert와 연동 없음** — warning/error가 항상 콘솔만 (미션 3: level별 페이지 출력 제어 필요)

### 7.8 UIComponents

- 정적 카테고리 벡터: 배송, 품질, 가격, 서비스, 사용성
- 필터 `<select>` 옵션 생성에만 사용, 로직과 분리 수준은 낮음

### 7.9 FileHandler

- `saveResult` / `save`: 콘솔 출력만
- 라우트·파일 I/O·다운로드와 **연동 없음** → Lava Flow

---

## 8. 의도적 코드 스멜·안티패턴

### 8.1 코드 스멜

| 스멜 | 현재 코드 예시 |
|------|----------------|
| 긴 함수 | `main()`, `renderPage()` |
| 중복 코드 | `containsAny()` (TextAnalyzer, Filters), 감정 키워드(Constants vs Filters) |
| 부적절한 네이밍 | `fil`, `sent`, `kw`, `res2`, `tmpFiltered` |
| 전역 변수 | `fil_data`, `globalSent`, `globalKw`, Session static |
| 매직 넘버/하드코딩 | 키워드 map, 포트 8080 |
| 테스트 미비 | 테스트 디렉터리·프레임워크 없음 |

### 8.2 안티패턴

| 안티패턴 | 반영 방식 |
|----------|-----------|
| God Function | main.cpp에 서버·HTML·파싱·라우팅 집중 |
| Spaghetti Code | 파일 간 의존성·데이터 흐름 불명확 |
| Feature Envy | Filters가 Constants 내부 map 구조에 강결합 |
| Shotgun Surgery | 키워드 변경 시 Constants, Filters, UIComponents 다수 수정 |
| Lava Flow | FileHandler, Session 미사용 멤버 |

---

## 9. 미션 안내 분석 (project_purpose.md §6)

총 **8단계**, 예상 **약 13시간**.

| 단계 | 시간 | 미션 내용 | 현재 코드 상태·권장 작업 |
|------|------|-----------|---------------------------|
| **1** | 1h | 프로젝트 개요·실습 준비, 전체 미션 안내 | 본 보고서로 대체 가능 |
| **2** | 2h | 테스트 구조 개선, coverage 90%+ | 테스트 **0건** → GoogleTest 등 도입, 핵심 로직 단위 테스트 |
| **3** | 1.5h | 오류·UX 개선 | ① Logger level → 페이지 alert 연동 ② textarea multi-line ③ **중립 필터·키워드 main 스킵** 수정 |
| **4** | 1h | 네이밍·매직넘버·전역 변수 | `fil`→`filterBy...`, 전역→Session/서비스 객체로 캡슐화 |
| **5** | 1.5h | 긴 함수·중복 코드 | `HtmlRenderer`, `Router`, 공통 `KeywordMatcher` 추출 |
| **6** | 1h | 리팩토링 기법 1개 추가 | 감정 분석 전략 패턴, `CsvParser` 클래스 등 |
| **7** | 3h | 추가 요구사항 | Trend 시각화(`test_feedback_trend.csv` **미포함**), 감정 필터 File DB |
| **8** | 2h | 팀 리뷰·발표 | 상대 프로젝트 장단점 정리 |

### 9.1 단계별 우선순위 제안

```text
1단계(이해) → 2단계(테스트 골격) → 3단계(버그·UX) → 4~6단계(리팩토링)
→ 7단계(기능 확장) → 8단계(리뷰)
```

테스트를 먼저 두면 3단계 필터·로그 수정 시 회귀를 막기 쉽습니다.

### 9.2 문서화된 리팩토링 기법 (§5.2)

| 기법 | 적용 예 |
|------|---------|
| Extract Function | HTML 렌더링, 라우트 핸들러 분리 |
| Extract Class | `HtmlRenderer`, `Router`, `CsvParser` |
| Replace Conditional with Polymorphism | 감정 분석 전략 |
| Move Method | 관련 메서드를 적절한 클래스로 이동 |
| Improve Data Class | Feedback에 분석 결과·setter |
| Package by Feature | `handlers/`, `services/`, `models/` |

---

## 10. 개선 포인트 및 리스크

### 10.1 즉시 수정 권장 (미션 3)

| # | 항목 | 설명 |
|---|------|------|
| 1 | 중립 필터 | TextAnalyzer·Filters 감정 판별 로직 **단일 소스**로 통합 |
| 2 | 키워드 필터 | `fil()`에서 `"main"` 서브키도 매칭하도록 수정 |
| 3 | 로그 UI | warning/error를 페이지에 표시할지 설정 가능하게 |
| 4 | textarea | multi-line 입력 UX (rows, placeholder 등) |

### 10.2 구조 개선 (미션 4~6)

- `main.cpp` 분리: 라우팅 / HTML / 애플리케이션 서비스 계층
- Constants 단일화: Filters의 `S_KEYWORDS` 제거 또는 Constants 참조만 사용
- FileHandler: 제거 또는 다운로드·DB 저장에 실제 연동
- Session: 미사용 map 제거, 명확한 세션 API

### 10.3 기능 확장 리스크 (미션 7)

| 항목 | 리스크 |
|------|--------|
| Trend 시각화 | `test_feedback_trend.csv` 샘플 파일 저장소에 없음 — 제공·형식 정의 필요 |
| File DB | 스키마·동시성·백업 정책 미정 — 요구사항 명세 선행 권장 |

---

## 11. 결론

FeedbackAnalyzer_08은 **피드백 분석 웹앱 형태의 리팩토링 실습 프로젝트**입니다. 기능은 단순하지만 **main.cpp 중심의 God Object 구조**, **이중 키워드·감정 로직**, **전역 상태**, **테스트 부재**가 핵심 과제입니다.

`project_purpose.md`의 8단계 미션은 **테스트 기반 안정화 → 버그 수정 → 구조 리팩토링 → Trend/DB 확장 → 팀 리뷰** 순으로 설계되어 있으며, 본 보고서의 모듈·라우트·스멜 분석을 기준으로 단계별 작업 범위를 삼을 수 있습니다.

---

## 부록 A. 참고 파일 목록

| 파일 | 용도 |
|------|------|
| `project_purpose.md` | 목적, 스멜, 미션 로드맵 |
| `README.md` | 빌드·실행·CSV 형식 |
| `CMakeLists.txt` | 빌드 설정 |
| `src/cpp/main.cpp` | 서버·UI·라우팅 |
| `src/cpp/TextAnalyzer.h` | 감정·키워드 집계 |
| `src/cpp/Filters.cpp` | 필터·별도 S_KEYWORDS |

## 부록 B. 용어

| 용어 | 설명 |
|------|------|
| cpp-httplib | 헤더 온리 C++ HTTP 서버/클라이언트 라이브러리 |
| God Function | 한 함수가 과도한 책임을 가짐 |
| Lava Flow | 더 이상 쓰이지 않는 코드 잔존 |
| Shotgun Surgery | 한 변경이 여러 파일 수정을 유발 |

---

*본 문서는 코드베이스 정적 분석 및 `project_purpose.md` 기준으로 작성되었습니다.*
