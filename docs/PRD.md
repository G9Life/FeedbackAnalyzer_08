# Feedback Analyzer — PRD (Phase 5 기준)

**문서 버전**: Phase 5 (Epic · Journey · Story · Gherkin · README 동기화)  
**범위**: `C:\DEV\FeedbackAnalyzer_08` 학습 저장소 (`src` 리팩토링 + 문서·운영)  
**금지**: 코드 · 클래스 설계 · 빌드 스크립트 (본 PRD는 요구·계약·문서 정책만 정의)

---

# 1. 프로젝트 개요

## 1.1 한 줄 목적문 (What / Who / Why)

**What**: 규칙 기반 피드백 감정·카테고리 분석을 **단일 Registry·Converter**와 **검증 가능한 입출력 계약**으로 제공하는 C++ 웹 학습 시스템.  
**Who**: C++17·클린 아키텍처·TDD를 실습하는 중급 학습자.  
**Why**: 레거시(단일 main·if-else·이중 규칙)에서 **계약·테스트·BCE 레이어 분리**로 전환해, 집계·필터·출력 불일치를 제거하고 회귀를 증명 가능하게 하기 위함.

**문서 이중 정체 (README 정책)**: [README.md](../README.md) **상단(1~70행)** 은 **현행 제품 표면**(설치·빌드·실행·CSV·`src/cpp` 트리)을, **하단(72행~)** 은 **리팩토링 목표·계약**(BCE·Registry·Catch2·PRD §9)을 기술한다. 본 PRD는 **학습·리팩토링 시스템**을 정본으로 하며, 상단은 **레거시 현행 스냅샷**으로만 취급한다(§9.1~9.2).

## 1.2 배경 및 문제 정의

| 현상 | 원인 (현재 코드) | 비즈니스·학습 영향 |
|------|------------------|-------------------|
| 동일 피드백의 감정 라벨·건수가 경로마다 다름 | `TextAnalyzer`와 `Filters`가 **서로 다른 키워드·우선순위** 사용 | 대시보드·필터·CSV 신뢰 불가 |
| 실패 시 빈 화면·모호한 경고 | 단일 `main`에 HTTP·HTML·분류·전역 상태 혼재, **입력/오류 계약 부재** | RED 테스트·재현 불가 |
| 키워드 변경 시 다파일 수정 | `Constants`·`Filters` **if-else·하드코딩**, Shotgun Surgery | OCP·SRP 학습 목표 달성 불가 |
| 출력 채널마다 다른 가정 | HTML·CSV·(목표)JSON이 **동일 AnalysisSnapshot** 없이 직렬화 | 직렬화 버그·회귀 추적 불가 |

**현재 구조 요약**: 단일 `main()` 진입, 문자열→if-else 키워드 분기, 전역 `fil_data`·`globalSent` 등 mutable 상태, Domain/Boundary 미분리.

## 1.3 목표 (측정 가능)

| ID | 목표 | 측정 |
|----|------|------|
| G-01 | 집계·필터·단건 `classify` 결과 **100% 일치** | 골든 N≥20, IT 불일치 0건 (Epic SC-04, INV-D1, D6) |
| G-02 | Domain·Boundary·Data **커버리지 하한** 충족 | Domain line ≥95%/branch ≥90%; Boundary line ≥85%; Data line ≥90%; overall line ≥90% |
| G-03 | Catch2·Gherkin **전 시나리오 GREEN** | `[domain]` `[boundary]` `[data]` pass 100%, GH-01~13·README-01~08 매핑 누락 0 (SC-03, SC-06) |
| G-04 | **error.code**·스키마 계약 동결 | 미문서화 code 변경 0건; 실패 시 `field` 포함 (SC-05) |
| G-05 | 금지 패턴 제거 | 이중 SENTIMENT 0, 전역 mutable 분석 상태 0, Entity 내 디버그 cout 0 (SC-07) |

## 1.4 비목표

| ID | 비목표 | 사유 |
|----|--------|------|
| NG-01 | ML·딥러닝 기반 감정 분석 | Phase 4는 **규칙·계약·아키텍처** 학습이 핵심 |
| NG-02 | Trend 시각화·File DB·프로덕션 배포·인증·다중 테넌트 | 저장소 범위 외 (별도 Epic). [project_purpose.md](../project_purpose.md) 후반 **8단계 미션** 중 Trend·File DB·배포 등은 **Phase 5 v1.0 비목표** — 워크숍·AI Activities(§9.10)에서도 **참고만**, AC·Gherkin GREEN 범위에 **포함하지 않음** |
| NG-03 | 알고리즘 정확도 경진(대규모 말뭉치 벤치마크) | 성공 기준은 **불변식·테스트·커버리지** |

---

# 2. 사용자 및 이해관계자

## 2.1 타깃 사용자 (페르소나 1명)

| 항목 | 내용 |
|------|------|
| **이름** | 김리팩 (가명) |
| **역할** | 중급 C++ 개발자, 사내 리팩토링·TDD 과제 수행 |
| **목표** | “돌아가는 UI”가 아니라 **계약·Catch2·레이어**로 분석 파이프라인 증명 |
| **불편** | 감정 통계와 필터 결과 불일치, 테스트 없이 if-else 수정 |
| **성공** | SC-01~07·**Level 5 체크리스트 42항**([01_요구사항_서술_패키지.md](./01_요구사항_서술_패키지.md) Level 5) 통과, 팀 리뷰에서 Invariant·Gherkin 시연 |

**이해관계자**: 강사/리뷰어(완료 증거 채점), 동료 학습자(코드 리뷰)— PRD 범위에서는 **검증 권한**만 명시.

## 2.2 주요 시나리오 (Phase 4 Journey 기반)

### 시나리오 A — 문제 인식 → 계약 정의 (J1 → J2)

학습자가 로컬 `http://localhost:8080`에서 동일 문장으로 analyze·filter·download를 비교해 불일치를 재현하고, Input/Output/Error schema·Hub·우선순위 표를 확정한다.  
**성공 신호**: `EMPTY_TEXT`, `UNKNOWN_EMOTION` 등 **기계 검증 가능** 실패 목록 존재.

### 시나리오 B — Dual-Track TDD (J3 → J4)

Domain에서 RED(`GH-01`~`09`) → GREEN(Registry+Converter) 후, Boundary는 Fake Registry로 Validator·Presenter만 검증한다.  
**성공 신호**: HTTP 없는 Domain 테스트만으로 INV-D1·D5·D6 통과.

### 시나리오 C — README E2E + 회귀 (J5 → J6 → J7)

README-01~04(접속·CSV·시각화·필터·다운로드) 통과, 커버리지·Matrix·골든 게이트 실행 후 리뷰 제출.  
**성공 신호**: README-04 CSV 1행 = 부정+배송 필터 1건.

---

# 3. 기능 요구사항

> 템플릿의 “meter 허브·콘솔”은 본 제품에서 **감정 Hub(중립)·구조화 출력(HTML/JSON/CSV)** 으로 해석한다. `std::cout` 디버그는 **금지**(비기능·금지 패턴).

## 3.1 기능 목록

| 구분 | 기능 ID | 기능명 | 추적 |
|------|---------|--------|------|
| **필수** | F-01 | 입력 검증 (text·라벨:본문·CSV) | ST-01, GH-05~07, README-08 |
| **필수** | F-02 | 감정 Hub 기반 형식 변환 (긍·부·중 1개) | ST-02, GH-01~02, INV-D2,D3 |
| **필수** | F-03 | 분석 결과 출력 (HTML 대시보드·통계·목록) | README-01,03, GH-10; Logger level은 권장 |
| **권장** | F-04 | OCP UnitRegistry (단일 규칙 공급) | ST-03, INV-D8 |
| **권장** | F-05 | 동적 Unit 등록 (`registerUnit`) | ST-06 |
| **권장** | F-06 | 감정·카테고리 필터 | GH-08~09, README-04,07 |
| **선택** | F-07 | JSON 스냅샷 출력 | ST-04, GH-11 |
| **선택** | F-08 | CSV 다운로드 | ST-04, GH-12, README-04 |
| **선택** | F-09 | HTML 표·escape | ST-04, GH-13 |
| **선택** | F-10 | 설정 외부화 `loadRatios` (YAML/JSON) | ST-05 |

## 3.2 기능별 입출력 계약 (문자열 계약)

### F-01 입력 검증

| 방향 | 계약 |
|------|------|
| In | `POST /analyze` body: `text=<UTF-8>`; trim 후 length≥1 |
| In | 선택 파싱: `<sentimentId>:<body>` — `:` 1개, 라벨·본문 비공백 |
| In | `POST /upload` multipart `file`; CSV 첫 행 헤더에 `text` |
| Out OK | HTTP 200, 세션 건수 증가 |
| Out Fail | `{ "error": { "code", "message", "field", "httpStatus" } }` — 예: `EMPTY_TEXT` 400 `field=text`; `CSV_MISSING_TEXT_COLUMN` 422 `field=file` |

### F-02 Hub 감정 변환

| 방향 | 계약 |
|------|------|
| In | `Feedback.text` (비어 있지 않음) |
| Out | `AnalysisResult.sentimentId` ∈ {`긍정`,`부정`,`중립`}; 미매칭 → Hub `중립` |
| Out | 우선순위 적용 후 **정확히 1개** sentimentId |

### F-03 HTML 출력

| 방향 | 계약 |
|------|------|
| In | `AnalysisSnapshot` (sentimentCounts, categoryCounts, feedbacks[]) |
| Out | `text/html; charset=UTF-8`; 섹션 `감정 분포`, 키워드/카테고리 통계; 피드백 테이블 |

### F-04~F-06 Registry·필터

| 기능 | In | Out |
|------|----|-----|
| F-04 | `loadRatios(path)` 또는 in-memory bundle | `Status::Ok` + Registry replaceAll |
| F-05 | `registerUnit(type, unitId, keywords[])` | Ok / `DUPLICATE_UNIT` |
| F-06 | `POST /filter` `sentiment`, `keyword` ∈ enum∪{`전체`} | 필터된 `Feedback[]`; 집계는 **부분집합**만 |

### F-07 JSON

| Out | `{ "sentimentStats":[{"id","count"}], "categoryStats":[...], "feedbacks":[{"text"}] }` |

### F-08 CSV

| Out | `Content-Disposition: attachment; filename="filtered_feedback.csv"`; line1 `text`; data rows = fil_data count |

### F-10 설정 로드

| Fail codes | `CONFIG_PARSE_ERROR`, `DUPLICATE_UNIT`, `INVALID_HUB_CONFIG`, `UNSUPPORTED_CONFIG_VERSION` |

## 3.3 제약 사항 (Gherkin Background Given과 일치)

### Domain Background (통합 Feature)

| 제약 ID | 규칙 | Gherkin Given |
|---------|------|---------------|
| C-D01 | UnitRegistry = **기본 ratios 스냅샷**으로 초기화 | `Given UnitRegistry가 기본 ratios 스냅샷으로 초기화되어 있다` |
| C-D02 | 감정 Hub = **`중립`** | `And 감정 Hub는 "중립"이다` |
| C-D03 | 감정 우선순위 = **`긍정` → `부정` → `중립`** | `And 감정 우선순위는 "긍정" 다음 "부정" 다음 "중립"이다` |
| C-D04 | 카테고리 Unit = **`배송`,`품질`,`가격`,`서비스`,`사용성`**, each **main** keyword group | `And 카테고리 Unit "배송",... 이 main 키워드 그룹을 가진다` |
| C-D05 | 피드백당 감정 **1개**; 미매칭 → Hub | Rule in Feature |
| C-D06 | 카테고리 건수는 **main** 매칭 시만 증가 | Rule in Feature |

### Boundary Background (README Feature)

| 제약 ID | 규칙 | Gherkin Given |
|---------|------|---------------|
| C-B01 | 서버 base URL = **`http://localhost:8080`** | `Given the Feedback Analyzer server is running at "http://localhost:8080"` |
| C-B02 | 초기 세션 피드백 = **empty** | `And the session feedback list is empty` |

### 입력 형식

- 텍스트: UTF-8, 최대 10 000자(권장 상한), trim 적용.
- CSV: 헤더 **`text`** 필수; 데이터 행 첫 필드 = 본문.
- 필터: `sentiment` ∈ {`전체`,`긍정`,`부정`,`중립`}; `keyword` ∈ {`전체`} ∪ Registry category ids.

### 허용 단위 (감정·카테고리)

| type | 허용 unitId |
|------|-------------|
| sentiment | `긍정`, `부정`, `중립` (Hub) |
| category | `배송`, `품질`, `가격`, `서비스`, `사용성` |

### “음수” 정책 (본 도메인)

- **건수·count**: 0 미만 **금지**; 통계·JSON·HTML에 음수 표시 0건.
- **피드백 건수**: 필터·집계 후 합계는 0 이상 정수; 빈 집합은 0건 표시(VIZ-01: 필드 누락 없이 0).
- (물리 단위 음수는 **해당 없음**.)

### 미지원 단위·라벨 처리

| 상황 | 처리 | error.code |
|------|------|------------|
| 파싱 라벨 `화남` 등 미등록 sentiment | **거부**, Hub 치환 금지 | `UNKNOWN_EMOTION` |
| 필터 `keyword=해외배송` | **거부**, 세션 불변 | `UNKNOWN_CATEGORY_FILTER` |
| 필터 미등록 sentiment | **거부** | `UNKNOWN_SENTIMENT_FILTER` |
| `registerUnit` 중복 | **거부**, Registry 스냅샷 유지 | `DUPLICATE_UNIT` |

---

# 4. 비기능 요구사항

## 4.1 기술 스택

| 항목 | 버전·도구 |
|------|-----------|
| 언어 | C++17 이상 |
| 빌드 | CMake 3.14+ |
| HTTP (Boundary) | cpp-httplib |
| 테스트 | Catch2 v3, 태그 `[domain]` `[boundary]` `[data]` `[integration]` |
| 포맷 | clang-format (`.clang-format`) |

## 4.2 아키텍처 원칙

| 원칙 | 적용 |
|------|------|
| **SRP** | 판별(Entity)·조율(Control)·HTTP/검증/표현(Boundary) 분리 |
| **OCP** | 새 sentiment/category는 Registry 등록·Presenter 추가; Converter 분기 확장 금지 |
| **BCE** | **B**: Validator, Presenter, ErrorMapper · **C**: Analyze, Upload, Filter, Export · **E**: Feedback, UnitRegistry, Converter |
| **의존성** | `boundary → control → entity`; entity는 httplib·filesystem·HTML 미의존 |

## 4.3 테스트 커버리지 목표

| 레이어 | line | branch |
|--------|------|--------|
| Domain | ≥ 95% | ≥ 90% |
| Boundary | ≥ 85% | ≥ 80% |
| Data | ≥ 90% | ≥ 85% |
| Overall | ≥ 90% | ≥ 85% |

## 4.4 확장성 원칙

1. 새 **감정·카테고리** Unit: `registerUnit` 또는 `loadRatios`만 변경; Converter·if-else 감정 분기 **추가 금지**.
2. 새 **출력 채널**: Presenter 구현체만 추가; `AnalysisSnapshot` 필드 **삭제·이름 변경 금지**(major 버전 제외).
3. 골든·Gherkin·`error.code` **동시 갱신** 없으면 Registry·우선순위 변경 **merge 금지**.

---

# 5. 데이터 요구사항

## 5.1 판별 규칙 유일 공급원 (DR-01)

- **내용**: 감정·카테고리 키워드·우선순위·Hub는 **UnitRegistry 단일 스냅샷**에서만 로드.
- **불변**: INV-D8 — TextAnalyzer/Filters 이중 테이블 **0건**.
- **추적**: ST-03, ST-05, ST-06 · GH-01~04.

## 5.2 세션·입력 데이터 (DR-02)

- **내용**: Boundary 검증 통과 후 `Feedback[]` 세션 저장; 필터 결과 `fil_data` 부분집합.
- **계약**: README-02(3건 CSV), README-03(단건 analyze).
- **추적**: ST-01 · README-02,03 · GH-06,07.

## 5.3 동적 Unit 등록·설정 로드 계약 (DR-03)

**범위**: 피드백 분석 **감정·카테고리 Unit**만. 길이·환산(meter/feet/cubit, 비율 `3.28084`/`1.09361`)은 **도메인 외**(README·§3 금지 패턴과 동일).

**현행(`src/cpp`)**: `Constants::init()` 하드코딩만 존재 — `registerUnit`·`loadRatios`·`config/ratios.yaml` **미구현**(목표: `src/data` + `UnitRegistry`).

### DR-03-A — 일괄 로드 `loadRatios` (F-10 · ST-05)

| 항목 | 계약 |
|------|------|
| In | 경로 `config/ratios.yaml` 또는 `tests/fixtures/registry_default.yaml`; UTF-8 YAML |
| 동작 | 파싱 성공 시 `UnitRegistry.replaceAll(snapshot)` — **기존 스냅샷 전체 교체** |
| Out OK | `Status::Ok`; 이후 `classify`·`aggregate`·`filter`가 **동일 스냅샷**만 사용 |
| Out Fail | Registry **변경 없음**; `error.code` 아래 표 |

**YAML 스냅샷 스키마 (정본 — README 하단·Gherkin Background와 동일)**

```yaml
version: 1
hub:
  sentiment: "중립"          # C-D02: 변경 불가(목표 v1.0)
priority:
  sentiment: ["긍정", "부정", "중립"]   # C-D03
units:
  - type: sentiment
    id: "긍정"
    keywords: ["좋아요", "만족", "감사"]
  - type: category
    id: "배송"
    keywordGroups:
      main: ["배송", "택배", "배달"]   # C-D04, C-D06: 집계·건수는 main만
```

| 검증 | 실패 code |
|------|-----------|
| YAML 파싱 불가 | `CONFIG_PARSE_ERROR` |
| `version` 미지원 | `UNSUPPORTED_CONFIG_VERSION` |
| Hub ≠ `중립` 또는 우선순위 ≠ C-D03 | `INVALID_HUB_CONFIG` |
| 동일 `(type, id)` 중복 | `DUPLICATE_UNIT` |

- **마이그레이션**: 레거시 `Constants.cpp` 키워드·`main` 그룹은 위 스냅샷 **초기값**으로 이전(서브 그룹 `time`/`type_` 등은 v1.0 Registry **미포함** — 집계 규칙은 **main만**).

### DR-03-B — 런타임 등록 `registerUnit` (F-05 · ST-03 · ST-06)

| 항목 | 계약 |
|------|------|
| In | `type` ∈ {`sentiment`, `category`}; `unitId` UTF-8 비공백; `keywords[]` 1개 이상 |
| 호출 | Control 유스케이스 또는 Domain API(HTTP 라우트는 Boundary); Entity **직접 HTTP 금지** |
| Out OK | Registry에 Unit **추가**; 직후 `classify`·`filter`에 반영; 골든·기존 Unit **GREEN 유지**(의도 변경 없는 한) |
| Out Fail | `DUPLICATE_UNIT` — `(type, unitId)` 중복 시 **이전 스냅샷 불변** |

**논리 형식 (문서·테스트 문자열 계약)**

```text
registerUnit(type=sentiment, unitId=만족, keywords=[만족, 만족스럽])
registerUnit(type=category, unitId=포장, keywordGroups.main=[포장, 박스])
```

| type | 필수 필드 | 제약 |
|------|-----------|------|
| `sentiment` | `id`, `keywords[]` | Hub·기본 3감정(`긍정`/`부정`/`중립`) **삭제·우선순위 변경은 `registerUnit`으로 불가** — `loadRatios` 전체 교체 또는 별도 Story |
| `category` | `id`, `keywordGroups.main[]` | 집계·필터 판별은 **main** 매칭만(C-D06); 서브 그룹은 v1.0 **등록 API 없음** |

### DR-03-C — 등록 후 불변 (Converter·테스트)

| ID | 규칙 |
|----|------|
| INV-D8 | `TextAnalyzer`/`Filters` 이중 키워드 테이블 **0건** — 등록 결과는 **Registry 단일 경로**만 |
| INV-D1 | 동적 등록 후에도 `classify(text)` = 필터·집계 감정 **일치** |
| INV-D6 | 필터 결과 집계는 **부분집합**만 |
| 파싱 | 미등록 라벨 `화남:…` → `UNKNOWN_EMOTION`; Hub로 **치환 금지** |

### DR-03-D — 금지·비목표

| 금지 | 비고 |
|------|------|
| `meter`, `feet`, `yard`, `cubit` | 감정·카테고리 Unit **아님** |
| `3.28084`, `1.09361`, `convert(…)` | 피드백 분석 계약 **무관** |
| Converter에 감정별 `if (id=="긍정")` 분기 추가 | OCP 위반 — 키워드는 Registry만 |
| `registerUnit`으로 Hub·`priority.sentiment` 단건 변경 | v1.0 **미지원** — `loadRatios` 스냅샷 사용 |

- **추적**: ST-03, ST-05, ST-06 · F-04, F-05, F-10 · GH-01~04(Background) · README 설정 절.

## 5.4 회귀·골든 픽스처 (DR-04)

- **내용**: `tests/fixtures/golden_feedbacks.yaml` N≥20; `classify` = `aggregate` 단건 = `filter` 판별 (INV-D1, D6, D3).
- **추적**: Epic SC-04 · ST-02 · GH-01,02,08,09.

---

# 6. 출력 요구사항

## 6.1 HTML 대시보드 (OR-01)

| 항목 | 요구 |
|------|------|
| 트리거 | `GET /`, `POST /analyze`, `POST /filter` (README-01,03,05,06,07) |
| 필수 UI | 텍스트 폼→`/analyze`; CSV→`/upload`; 필터→`/filter` (README-01) |
| 통계 | `감정 분포`: 긍·부·중 count; 카테고리: Registry id별 count |
| 불변 | 표시 집합 건수 = sentiment count 합 (INV-D5, VIZ-01); 필터 후 **부분집합** (INV-D6) |
| 보안 | 테이블·알림에 raw `<script>` 0건 (GH-13) |

## 6.2 JSON 출력 스키마 (OR-02)

| 경로 | 타입 | 필수 |
|------|------|------|
| `sentimentStats[]` | `{ "id": string, "count": int≥0 }` | ☑ |
| `categoryStats[]` | `{ "id": string, "count": int≥0 }` | ☑ |
| `feedbacks[]` | `{ "text": string }` | ☑ |
| `error` | `{ "code", "message", "field" }` | 실패 시만 |

- id는 Registry 등록 id만.
- **추적**: GH-11 · ST-04.

## 6.3 CSV 출력 스키마 (OR-03)

| 항목 | 값 |
|------|-----|
| MIME | `text/csv; charset=UTF-8` |
| 파일명 | `filtered_feedback.csv` |
| 헤더 | `text` (1열) |
| 데이터 | fil_data 각 행 1줄, 본문 UTF-8 |
| BOM | 선택 허용 |
| 빈 fil_data | `NO_DATA_TO_EXPORT`, HTTP 404 |

- **추적**: GH-12 · README-04.

## 6.4 표(HTML Table) 출력 스키마 (OR-04)

| 항목 | 요구 |
|------|------|
| 행 수 | = 화면 `feedbacks` 건수 |
| 열 | 최소 `text`; 권장 `sentimentId`, `categories` |
| escape | `<`, `>`, `"`, `&` 엔티티 변환 |
| 정합 | 행별 sentiment = `classify(text)` (INV-D1) |

- **추적**: GH-13 · ST-04.

---

# 7. 성공 지표

## 7.1 인수 기준 (체크박스 · 테스트 가능)

- [ ] **AC-01** 골든 N≥20: `classify`/`aggregate`/`filter` 감정 라벨 불일치 **0건** (SC-04, ST-02).
- [ ] **AC-02** Catch2 `[domain]` `[boundary]` `[data]` **전부 PASS**, skip 0 (SC-03).
- [ ] **AC-03** Domain coverage line ≥95%, branch ≥90% (SC-01).
- [ ] **AC-04** Gherkin **GH-01~13, README-01~08** 각각 Catch2 또는 IT에 1:1 매핑 (SC-06).
- [ ] **AC-05** README-04: 부정+배송 필터 1건 → CSV 데이터 행 1건, 본문 배송 관련 (README-04, GH-08,12).
- [ ] **AC-06** `CSV_MISSING_TEXT_COLUMN`·`EMPTY_TEXT`·`UNKNOWN_EMOTION` HTTP·code·field 일치 (ST-01, README-08).
- [ ] **AC-07** 이중 SENTIMENT·전역 mutable 분석 상태 **0건** (SC-07).

## 7.2 회귀 보호 규칙 (계약 변경 금지 정책)

| 규칙 ID | 내용 |
|---------|------|
| R-01 | `error.code` 문자열 삭제·변경 시 → 테스트 + Gherkin + PRD/요구사항 패키지 **동시 PR** |
| R-02 | 골든 픽스처 기대 sentiment 변경 시 → 의도 주석 + GH/DT-ID 갱신 |
| R-03 | 공개 Domain API(`classify`, `aggregate`, `filter`) 시그니처 변경 → major 태그 |
| R-04 | RED 상태에서 디렉터리·대규모 리네이밍 **금지**; REFACTOR는 Catch2 전체 GREEN 후 |
| R-05 | 레거시 Filters/TextAnalyzer 키워드를 **기대 기준**으로 사용 **금지** |
| R-06 | Background Given C-D01~D04, C-B01~B02 변경 시 → **전 Feature** 영향 분석 필수 |

---

# 8. 용어 정의 (Glossary)

| 용어 | 정의 |
|------|------|
| **Feedback** | 분석 대상 비어 있지 않은 UTF-8 텍스트 1건. |
| **UnitRegistry** | 감정·카테고리 Unit·키워드·우선순위·Hub를 보유하는 **유일 규칙 저장소**. |
| **Hub (감정)** | 미매칭 시 귀속되는 감정 Unit; Phase 4에서 **`중립`**. |
| **Converter** | Registry만 사용해 `classify`·`aggregate`·`filter`를 수행하는 Entity 서비스. |
| **AnalysisSnapshot** | sentiment/category 집계 맵 + feedback 목록; Presenter의 **단일 입력 DTO**. |
| **error.code** | 기계 검증 가능한 실패 식별자(예: `EMPTY_TEXT`); UI·테스트·문서에서 **동일 문자열** 사용. |
| **fil_data** | 마지막 필터 결과 부분집합; CSV 다운로드·필터 후 HTML의 **표시 기준** 세션. |
| **Dual-Track** | Domain 테스트(HTTP 없음)와 Boundary 테스트(Fake Registry)를 **분리**하는 TDD 방식. |
| **골든 픽스처** | `golden_feedbacks.yaml`에 고정된 text·기대 sentiment; SC-04 회귀 기준. |
| **Phase 4** | Epic·Journey·Story·Gherkin(GH·README)·Level 5 체크리스트가 정의된 요구사항 단계. |
| **Phase 5** | 본 PRD·README 하단·Level 5 42항·레거시→BCE 리팩토링 v1.0 범위. |
| **Lava Flow** | `FileHandler` 등 **선언만 있고 실질 기능 없는** 레거시 잔재(§9.6). |
| **HtmlPresenter** | 목표 Boundary 출력 컴포넌트; 현행 `main.cpp` `renderPage()` 대체 대상(§9.5). |

---

# 9. 문서·저장소·운영 (README Phase 5 동기화)

> README에만 있던 항목(비교 2항·범위 초과 위험)을 PRD에 **편입**하여 이중 정체·오해를 관리한다. README 상단은 **수정 금지** 정책이 있을 수 있으므로, 불일치 시 **본 절·PRD가 우선**한다.

## 9.0 README 전용·상단 항목 — PRD 반영 요약

| README 항목 (상·하단) | PRD 반영 절 | Phase 5 v1.0 취급 |
|----------------------|-------------|-------------------|
| 상단 제품 소개·주요 기능 6항 | §9.1·§9.2 | **현행 스냅샷**; 인수·F-ID **아님** |
| `feedback_analyzer.png` | §9.1 | 문서 자산; 요구사항 **아님** |
| `feedback_analyzer_cpp`·상단 트리 | §9.3·§9.4 | **역사적 예시**; 정본 `FeedbackAnalyzer_08` |
| `UIComponents`·`Logger` (상단 트리) | §9.4 | 레거시 파일; 리팩토링 시 **분리·정리 대상** |
| `FileHandler.h` “파일 처리” | §9.6 | **Lava Flow**; 기능 설명과 **불일치** |
| `rmdir`·MinGW 고정 빌드 명령 | §9.7 | **개발자 로컬 예시**; CI·AC **비규범** |
| AI·예시 **README-01~04**만 | §9.10 | 워크숍 **최소**; 인수는 **README-01~08**(AC-04) |
| `registry_default.yaml` (README) | §9.13 | Registry fixture; 골든은 **`golden_feedbacks.yaml`** |
| MIT **전문** (README 하단) | §9.12 | 법적 전문은 README; PRD는 **MIT 적용**만 |
| `renderPage`·`main.cpp` (README 하단) | §9.5 | **Before** 명칭; 목표는 Presenter |

## 9.1 README 이중 구조 (문서 정책)

| 구간 | 행(대략) | 톤 | PRD 역할 |
|------|----------|-----|----------|
| **상단** | 1~70 | 현행 **제품** — 설치·빌드·실행·사용법·CSV | 레거시 **현행 스냅샷**; Gherkin/AC **완료 기준 아님** |
| **구분선** | 72 | `---` | — |
| **하단** | 73~419 | **리팩토링·계약·BCE·테스트·AI·기여** | Phase 5 **목표·계약**; 본 PRD와 **1:1 추적** |

- 상단: “돌아가는 UI” 기준 설명. **집계·필터·CSV 불일치는 RED로 정상**(§1.2).
- 하단: “계약·Catch2·Registry” 기준. **목표 구조** `src/boundary|control|entity|data`, `tests/`.
- README 말미: *상단=제품 소개(레거시 현행); 하단=리팩토링 목표·계약* — 본 PRD와 **동일**.

**상단 전용 자산 (요구사항 아님)**

| 항목 | README 위치 | PRD |
|------|-------------|-----|
| 배너 이미지 | `![feedback_analyzer](./feedback_analyzer.png)` | 저장소 루트 **선택 문서 자산**; UI·AC·테스트 **무관** |
| 제품 한 줄 소개 | L4~5 | §9.2와 동일 취지; **학습용 레거시** 전제 |

## 9.2 제품 표면 vs 학습 시스템

| 관점 | 설명 | Phase 5 v1.0 |
|------|------|----------------|
| **제품 표면** | 고객 피드백 수집·분류·시각화·CSV (README 상단·`project_purpose` §2) | 레거시 `src/cpp`로 **실행 가능** |
| **학습 시스템** | TDD·BCE·단일 Registry·error.code·커버리지·Gherkin (본 PRD) | **인수 기준**은 여기만 |

**README 상단 “주요 기능” 6항 (현행 스냅샷 — PRD §3 F-ID와 1:1 아님)**

| # | README 상단 문구 | Phase 5 대응(목표) | 비고 |
|---|------------------|-------------------|------|
| 1 | 텍스트 피드백 입력 (수동/CSV) | F-01, README-02~03 | |
| 2 | 키워드 기반 피드백 분류 | F-02, F-04, GH-01~04 | |
| 3 | 감정 분석 (긍/부/중) | F-02, Hub `중립` | |
| 4 | 피드백 필터링 및 검색 | F-06, README-04,07 | |
| 5 | 분석 결과 시각화 | F-03, OR-01 | Trend 차트 **비목표**(NG-02) |
| 6 | 결과 CSV 다운로드 | F-08, OR-03 | |

**범위 초과 방지**: 상단만 읽고 “완성된 제품”으로 오해하지 않는다. 인수는 **AC-01~07·Level 5 42항**·**README-01~08**만.

## 9.3 저장소·클론 경로 명명

| 항목 | README 상단(레거시 문구) | PRD 정본 |
|------|-------------------------|----------|
| 저장소 디렉터리 | `feedback_analyzer_cpp` (예시) | **`FeedbackAnalyzer_08`** (실제 워크스페이스·Git 루트) |
| 클론 URL | `[repository-url]` 플레이스홀더 | 배포자가 실제 URL로 교체 |
| 클론 후 `cd` | `cd feedback_analyzer_cpp` | **`cd FeedbackAnalyzer_08`** (또는 클론한 폴더명) |

- 문서·CI·Gherkin 경로는 **`FeedbackAnalyzer_08` 기준**으로 통일한다.
- README 상단 예시명은 **역사적 예시**이며, PRD·요구사항 패키지와 **불일치 시 PRD 우선**.

## 9.4 저장소 디렉터리 구조 (전체)

README 상단 트리는 `src/cpp`·`CMakeLists.txt`만 나열한다(`docs/`, `src/features/`, `Report/`, `.cursorrules` **누락**). Phase 5 **전체 구조**는 아래와 같다.

```
FeedbackAnalyzer_08/
├── feedback_analyzer.png       # README 상단 배너 (선택, §9.1)
├── src/
│   ├── cpp/                    # 레거시 현행 — 아래 파일 목록
│   ├── boundary/               # 목표 — Validator, Presenter, ErrorMapper
│   ├── control/                # 목표 — Analyze, Upload, Filter, Export
│   ├── entity/                 # 목표 — UnitRegistry, Converter, Feedback
│   ├── data/                   # 목표 — loadRatios, fixtures
│   └── features/               # Gherkin 정본 (GH, README 시나리오)
├── tests/                      # 목표 — domain, boundary, data, integration
├── config/                     # 목표 — ratios.yaml
├── docs/                       # PRD, 요구사항 패키지, TODO, 스토리보드
├── Report/                     # 분석·보고 인덱스
├── .cursorrules                # TDD·BCE·forbidden·커버리지
├── project_purpose.md          # 8단계 미션·코드 스멜 (§9.8)
├── CMakeLists.txt
└── README.md                   # 상단 현행 + 하단 목표
```

**레거시 `src/cpp/` 파일 (README 상단 트리·PRD 정본)**

| 파일 | README 상단 설명 | PRD 취급 |
|------|------------------|----------|
| `main.cpp` | HTTP·라우팅 | God Function; `renderPage()` — §9.5 |
| `httplib.h` | cpp-httplib | Boundary 의존(목표: boundary만) |
| `Feedback.h` | 피드백 모델 | Entity `Feedback`로 이전 |
| `TextAnalyzer.h/cpp` | 텍스트 분석 | → `Converter` (이중 규칙 제거) |
| `Filters.h/cpp` | 필터링 | → `Converter::filter` |
| `UIComponents.h/cpp` | UI 컴포넌트 | 로직·HTML 혼재; Presenter·Control로 **분리** |
| `Session.h/cpp` | 상태 관리 | 전역 mutable 제거 — §1.3 G-05 |
| `Logger.h/cpp` | 로깅 | F-03 Logger level 권장; Entity `cout` **금지** |
| `Constants.h/cpp` | 상수 | → `UnitRegistry` / `loadRatios` |
| `FileHandler.h` | “파일 처리” | **Lava Flow** — §9.6 (실제 미사용) |

## 9.5 레거시 구현명 vs 목표명

| 역할 | 레거시 (현행 `src/cpp`) | 목표 (BCE) |
|------|-------------------------|------------|
| HTTP·HTML 한곳 | `main.cpp`, **`renderPage()`** | `boundary` 라우트 + **`HtmlPresenter`** |
| 감정·키워드 | `TextAnalyzer`, `Filters` (이중 규칙) | **`Converter`** + **`UnitRegistry`** |
| 상수 | `Constants.cpp` 하드코딩 | **`loadRatios`** / `config/ratios.yaml` |
| 필터 세션 | `fil_data`, `globalSent` | 세션 포트·**`AnalysisSnapshot`** |
| JSON/CSV | HTML 혼재·부분 구현 | **`JsonPresenter`**, **`CsvPresenter`** |

- PRD·Gherkin·AC는 **목표명**으로 서술한다.
- README가 `renderPage`·`main.cpp`를 언급하는 것은 **Before 스냅샷**이지, 최종 아키텍처 명칭이 **아님**.

## 9.6 Lava Flow — `FileHandler`

| 항목 | 내용 |
|------|------|
| 위치 | `src/cpp/FileHandler.h`, `main.cpp` include·`static FileHandler` |
| README 상단 | “파일 처리”로 **기능 있는 것처럼** 기재 | **오해 유발** — 실제는 미사용 |
| 상태 | [project_purpose.md](../project_purpose.md) §3·§4: **파일 저장 로직 미사용·죽은 코드** |
| 스멜 | **Lava Flow** — 상단 트리·include만 존재 |
| Phase 5 v1.0 | (a) 제거, 또는 (b) 실제 업로드/저장 요구 시 **별도 Story**·F-ID. **현재 AC 범위 밖** |
| 금지 | FileHandler를 **감정·Registry·CSV 계약**의 근거로 사용하지 않음 |

## 9.7 설치·빌드·실행 (현행 제품 톤)

README **상단·하단 Quick Start**에 동일 계열 명령이 **중복** 기재된다. 본 절은 **레거시 즉시 실행**용이며, 인수·CI의 **유일 빌드 스크립트가 아님**.

| 단계 | README 상단(예시) | PRD 정본 |
|------|-------------------|----------|
| 요구사항 | C++17+, CMake 3.14+ | 동일 |
| 클린 빌드(Windows) | `rmdir /q /s build` | **로컬 편의**; Linux/macOS는 `rm -rf build` 등 대체 |
| CMake 생성 | `-G "MinGW Makefiles"` `-DCMAKE_CXX_COMPILER=.../g++.exe` | **MinGW 예시**; MSVC·Clang 환경은 generator·compiler **치환** |
| 빌드 | `cmake --build build` | 동일 |
| 실행 | `build\feedback_analyzer.exe` | OS별 실행 파일명·경로 **상이** 가능 |
| 접속 | `http://localhost:8080` | C-B01 |

- 위 절차 성공 = **“레거시 빌드 가능”**이지 **AC-01~07·Catch2 GREEN·커버리지 게이트 충족 아님**.
- Catch2·`ctest`·§4.3 커버리지는 **목표 `tests/` 타깃** 빌드에만 적용.

## 9.8 `project_purpose.md` 8단계와 NG-02

| 구분 | 내용 |
|------|------|
| `project_purpose.md` | 초기 기능·코드 스멜·**8단계 리팩토링 미션** 로드맵 |
| Phase 5 v1.0 (본 PRD) | ST-01~06, F-01~10, GH·README, Level 5 **42항** |
| NG-02와의 관계 | 8단계 중 **Trend·File DB·프로덕션 배포** 등은 **v1.0 비목표** — 미션 문서는 **동기 부여·스멜 설명**용 |
| AI Activities (§9.10) | `project_purpose.md` **압축 워크숍**; 산출물은 Catch2·PRD 계약에 **한정** |

## 9.9 Level 5 체크리스트 42항

- 정의: [01_요구사항_서술_패키지.md](./01_요구사항_서술_패키지.md) **Level 5** (총 **42항**).
- 페르소나 **성공** = SC-01~07 + **42항 전부** 충족(§2.1).
- README·AI Activities “Level 5·커버리지”는 **본 패키지 Level 5**를 가리킨다 — PRD에 수치·ID가 없던 **42**를 본 절에서 **명시**.

## 9.10 생성형 AI 활용 Activities (6시간)

Legacy 리팩토링 워크숍. **선택 학습 활동**이며, NG-02 범위(Trend·File DB·배포) **미포함**.

| 시간 | Activity | AI 활용 | 산출물 |
|------|----------|---------|--------|
| 0.5h | 레거시 불일치 재현 | analyze/filter/compare 표 정리 | Before 목록 |
| 1h | 계약·RED 테스트 | PRD·Gherkin → Catch2 제목·`error.code` | RED 목록 |
| 1.5h | Domain GREEN | Registry+Converter, 이중 Rule 제거 검토 | `tests/domain` |
| 1h | Boundary | Validator/Presenter Fake, `main` 분리 | `tests/boundary` |
| 1h | README E2E·골든 | **README-01~08**(AC-04), golden N≥20 | fixtures·IT |
| 1h | 회귀·리뷰 | Level 5 **42항**·커버리지(§4.3) | 발표 노트 |

**README 하단 AI 표와의 차이**: README는 워크숍 압축으로 **README-01~04**만 적을 수 있음. **인수·AC-04·Gherkin 정본**은 **README-01~08 전부** — 워크숍 1h는 01~04 **최소**, 회귀 전 05~08·README-08(입력 검증) **필수**.

**규칙**: AI 생성 코드는 **Catch2 필수** · RED→GREEN→REFACTOR · `error.code` 변경 시 Gherkin·PRD·README 하단 **동시 갱신**(R-01).

## 9.11 기여 가이드·Issue (README 동기화)

PRD §7.2 R-01~R06에 더해, 기여자는 아래를 **준수**한다.

| # | 기여 단계 |
|---|-----------|
| 1 | [PRD.md](./PRD.md) · [src/features/](../src/features/) Gherkin에서 **계약** 확인 |
| 2 | **테스트 없이** Domain 규칙(키워드·우선순위·Hub) 변경 PR **금지** |
| 3 | `cmake --build build` **성공** (레거시 타깃) |
| 4 | (목표) `ctest --test-dir build` **전부 PASS** |
| 5 | 골든·Gherkin·PRD·요구사항 패키지 **동시** 갱신 |
| 6 | forbidden: 이중 SENTIMENT, 전역 mutable 분석 상태, Entity `cout` 디버그 **0건** |

**Issue 작성 (필수 포함)**

- 재현 입력(plain text 또는 CSV 일부)
- 기대/실제 `sentimentId` (또는 집계 건수)
- 관련 `error.code` (있는 경우)

## 9.12 라이선스

- **라이선스**: MIT License  
- **Copyright**: Feedback Analyzer contributors  
- **README**: [README.md](../README.md) §라이선스에 **전문**(Permission·Disclaimer 포함) 수록 — **법적 조항 정본은 README**  
- **PRD**: 전문 **미중복**; 배포·기여·포크 시 **MIT 적용** 및 README 전문 **동봉** 요구

## 9.13 Fixture 경로 — README vs PRD 정본

| 경로 | README 하단 | PRD·DR-04 정본 | 용도 |
|------|-------------|----------------|------|
| `tests/fixtures/registry_default.yaml` | 설정·Registry 테스트 fixture | Registry **초기 스냅샷** 로드 테스트 | ST-05, `loadRatios` |
| `tests/fixtures/golden_feedbacks.yaml` | (README 미기재) | 골든 **N≥20**, classify=aggregate=filter | AC-01, DR-04, G-01 |

- README만 `registry_default.yaml`을 적어도 **골든 파일 누락으로 AC-01 불충족** — 구현·문서 시 **둘 다** 유지하거나 README 하단에 `golden_feedbacks.yaml` **추가 권장**(상단 수정 금지 시 하단만).

---

## 관련 문서

| 문서 | 설명 |
|------|------|
| [01_요구사항_서술_패키지.md](./01_요구사항_서술_패키지.md) | Level 1~5 Epic·Story·Gherkin·체크리스트 |
| [01_사용자여정_스토리보드.md](./01_사용자여정_스토리보드.md) | Awareness~Outcome 스토리보드 |
| [../README.md](../README.md) | **상단** 현행 실행·CSV / **하단** 리팩토링·AI·기여·MIT |
| [../src/features/](../src/features/) | Gherkin 정본 |
| [../project_purpose.md](../project_purpose.md) | 코드 스멜·8단계 미션(§9.8, NG-02 경계) |
| [../Report/README.md](../Report/README.md) | 분석 보고 인덱스 |
| [../.cursorrules](../.cursorrules) | TDD·forbidden·커버리지 |
| [TODO_리팩토링_v1.md](./TODO_리팩토링_v1.md) | ST-01~06 작업 목록 |

---

*본 PRD는 Phase 5 요구사항·README 하단·Level 5 42항에 종속. 구현·코드 없음.*
