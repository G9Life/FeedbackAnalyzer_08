# Feedback Analyzer — 테스트 계획서

| 항목 | 내용 |
|------|------|
| **문서 ID** | TP-2026-05-22-02 |
| **역할** | 시니어 QA 리드 |
| **기술 스택** | C++17, CMake, Catch2 v3, ctest |
| **앵커 샘플** | `"배송이 너무 늦어요. 불만입니다."` |
| **학습 맥락** | [project_purpose.md](../project_purpose.md) §6.1 (테스트 구조·커버리지 90%+) |
| **정본 계약** | [README.md](../README.md) (Phase 5), [PRD.md](./PRD.md), [src/features/](../src/features/) |
| **현행 코드** | `src/cpp` 레거시 — 목표: BCE + `UnitRegistry` + `Converter` |

---

## 1. 목적·범위

### 1.1 목적

[project_purpose.md](../project_purpose.md)의 **테스트 가능한 아키텍처**·**TDD 실습** 목표에 맞춰, 앵커 샘플로 **비즈니스 분류(감정·카테고리)** 를 Catch2로 고정하고, 리팩토링 전후 **집계·필터·출력 불일치**(코드 스멜: 이중 `containsAny`, 전역 `fil_data`)를 회귀 없이 제거한다.

### 1.2 앵커 샘플 기대값 (골든)

| 구분 | 입력 | 기대 |
|------|------|------|
| **단건 분류** | `배송이 너무 늦어요. 불만입니다.` | `sentimentId` = **부정** |
| **카테고리 main** | 동일 | **배송** 1건 (`main` 키워드 `"배송"` 매칭) |
| **1건 집계** | analyze 1회 후 | `부정=1`, `배송=1`, 긍정·중립·기타 0 |
| **필터·CSV (확장)** | 부정 + 배송 | 필터 1건, CSV 데이터 행 1건 (AC-05) |

### 1.3 project_purpose.md와의 관계

| project_purpose 항목 | 본 계획 반영 |
|----------------------|--------------|
| §2.1 핵심 기능 (키워드 분류·감정·필터·CSV) | P0 Domain + P1 필터 + P3 E2E |
| §2.3 I/O 표 `"화가 납니다."` → 부정·배송 | **대비 경계** BND-D02 (레거시는 중립 RED) |
| §4.1 테스트 미비·중복·전역 상태 | EX-01, EX-05, P0 단일 Registry 검증 |
| §6.1 2단계 coverage **90% 이상** | §5 Overall ≥90%; Domain **95%+** (PRD 상한) |
| §6.1 3단계 필터 오류(`중립`) | EX-06, DOM-004, BND-F01 |

### 1.4 README·PRD 추적

| README 주요 기능 | PRD / Gherkin |
|------------------|---------------|
| 2번 키워드 기반 분류 | F-02, GH-03, C-D04·D06 |
| 3번 감정 분석 | F-02, GH-01~02, C-D02~D03 |
| 4번 필터링 | F-06, GH-08, README-04, AC-05 |

### 1.5 In / Out of Scope

| In | Out |
|----|-----|
| Catch2 Domain / Boundary / Data | Trend 시각화, File DB (project_purpose §6.1 7단계) |
| 앵커·경계·파싱 오류 | ML 감정 분석 |
| gcov/lcov 게이트 | 대규모 말뭉치 벤치마크 |

---

## 2. Catch2 단위 테스트 — 범위·우선순위

### 2.1 디렉터리·태그 (목표 구조)

```
tests/
├── domain/          # [domain]   — Converter, UnitRegistry (HTTP 없음)
├── boundary/        # [boundary] — Validator, Parser, Presenter, Router
├── data/            # [data]     — golden_feedbacks.yaml, registry fixture
└── integration/     # [integration] — localhost:8080 E2E
```

> **현황**: [CMakeLists.txt](../CMakeLists.txt)는 `feedback_analyzer`만 빌드. project_purpose §4.1 **테스트 미비** → 본 구조가 2단계 미션 산출물.

### 2.2 우선순위 매트릭스

| 우선순위 | 태그 | 대상 (목표) | 앵커 관련 | 추적 |
|----------|------|-------------|-----------|------|
| **P0** | `[domain]` | `Converter::classify`, `aggregate` | 부정·배송 1; `불만`/`배송` 키워드 | GH-01, GH-03 |
| **P0** | `[domain]` | `UnitRegistry` 기본 스냅샷 | Constants 이중 정의 제거 후 단일 공급 | C-D01, Shotgun Surgery 완화 |
| **P1** | `[domain]` | `Converter::filter` | 부정+배송 → 앵커 1건만 | GH-08, INV-D6 |
| **P1** | `[boundary]` | `FeedbackParser`, `InputValidator` | **`:` 없음** → `COLON_MISSING` | 03_feedback_input_validation |
| **P2** | `[boundary]` | `HtmlPresenter`, `CsvPresenter` | CSV 1행 = 앵커 본문 | GH-12, README-04 |
| **P2** | `[data]` | golden N≥20 | 앵커 행 포함 | AC-01 |
| **P3** | `[integration]` | `main.cpp` 라우트 | POST `/analyze`·`/filter`·`/download` | project_purpose §2.2 시나리오 |

### 2.3 P0 Catch2 테스트 케이스 (앵커 중심)

| ID | TEST_CASE 제목 (예) | When | Then |
|----|---------------------|------|------|
| DOM-001 | `[domain] classify anchor shipping complaint is negative` | `classify("배송이 너무 늦어요. 불만입니다.")` | `sentimentId == "부정"` |
| DOM-002 | `[domain] aggregate anchor single feedback` | `aggregate({앵커})` | `부정=1`, `배송=1` |
| DOM-003 | `[domain] category main only delivery` | 앵커 | `배송>=1`, 서브만 매칭 0 |
| DOM-004 | `[domain] filter negative shipping returns anchor` | 앵커+긍정 1건 | `filter(부정,배송).size()==1` |
| DOM-005 | `[domain] classify is idempotent` | `classify` 2회 | 동일 id |
| DOM-006 | `[domain] contrast purpose doc near-negative` | `"배송이 너무 늦어요. 화가 납니다."` | 목표 부정 / 레거시 중립 RED |

### 2.4 Dual-Track TDD (project_purpose §6.1 2단계)

1. **RED** — P0 Domain만 (God Function `main` 비의존).
2. **GREEN** — 앵커·BND-D01 통과.
3. **Boundary** — Parser/Validator + Fake Registry.
4. **REFACTOR** — `Extract Class` (`Router`, `HtmlRenderer`) 후 Catch2 전체 GREEN.

---

## 3. 경계값 케이스 목록

### 3.1 앵커 파생 (Domain)

| ID | 입력/조건 | 기대 (목표) | 비고 |
|----|-----------|-------------|------|
| BND-D01 | `배송이 너무 늦어요. 불만입니다.` | 부정, 배송 1 | **앵커 Happy path** |
| BND-D02 | `배송이 너무 늦어요. 화가 납니다.` | GH-01: 부정 | project_purpose §2.3 표; 레거시 **중립** RED |
| BND-D03 | `전반적으로 무난했습니다.` | 중립 (Hub) | |
| BND-D04 | 긍·부 키워드 공존 | 긍정 우선 | C-D03 |
| BND-D05 | 서브만 `택배` (main 없음) | 배송 0 | C-D06 |
| BND-D06 | `배송은 빠른데 가격이 비싸요` | 배송≥1, 가격≥1 | GH-04 |
| BND-D07 | 빈 `feedbacks` | 전 count 0 | |
| BND-D08 | `  배송이 너무 늦어요. 불만입니다.  ` | trim 후 BND-D01 동일 | |

### 3.2 입력 파싱·형식 (Boundary) — `:` 없는 입력 포함

| ID | 입력/조건 | 기대 code | HTTP | field | 세션 |
|----|-----------|-----------|------|-------|------|
| **BND-P01** | **`만족합니다`** (라벨:본문 모드, **`:` 없음**) | **`COLON_MISSING`** | 400 | text | **0건, 불변** |
| BND-P02 | `긍정:만족합니다` | 성공 | 200 | — | +1 |
| BND-P03 | `부정:` | `EMPTY_BODY` | 400 | text | 0 |
| BND-P04 | `:본문만` | `EMPTY_LABEL` | 400 | text | 0 |
| BND-P05 | `화남:짜증납니다` | `UNKNOWN_EMOTION` | 400 | text | 0 |
| BND-P06 | `` / `\t\n` | `EMPTY_TEXT` | 400 | text | 0 |
| BND-P07 | `부정:본문:추가` | `INVALID_LABEL_FORMAT` (계약 확정) | 400 | text | 0 |
| BND-P08 | CSV `foo,bar` | `CSV_MISSING_TEXT_COLUMN` | 422 | file | 0 |
| BND-P09 | 앵커 일반 POST (콜론 없음) | 200, 부정·배송 1 | 200 | — | +1 |

> **BND-P01 vs BND-P09**: 콜론 필수는 **선택 파싱 모드**만. 앵커는 **단순 text** (`POST text=...`)로 검증.

### 3.3 필터·출력 경계

| ID | 조건 | 기대 |
|----|------|------|
| BND-F01 | 부정+배송, 앵커만 | 1건 |
| BND-F02 | 부정+배송, 앵커 없음 | 0건 |
| BND-F03 | `keyword=해외배송` | `UNKNOWN_CATEGORY_FILTER` |
| BND-F04 | `fil_data` 빈 `/download` | `NO_DATA_TO_EXPORT` 404 |
| BND-F05 | 필터 `중립`+`전체` (purpose §6.1 버그) | 집계 중립 건과 **일치** (수정 후 GREEN) |

---

## 4. 예외·특이 케이스 목록

| ID | 유형 | 시나리오 | 기대 | project_purpose / 레거시 |
|----|------|----------|------|---------------------------|
| EX-01 | 이중 규칙 | 앵커 analyze → 부정 필터 | 집계=필터 1건 | `TextAnalyzer` vs `Filters` 중복 |
| EX-02 | Hub 치환 금지 | `화남:…` | 400, 세션 0 | |
| EX-03 | 조용한 실패 | 빈 text | `EMPTY_TEXT` 명시 | 테스트 미비로 미검출 |
| EX-04 | God Function | `main` 예외 catch | 계약 JSON/HTML 오류 | §4.2 God Function |
| EX-05 | 전역 mutable | analyze 2회 | 누적 정책 명시 | `fil_data`, `globalSent` |
| EX-06 | 필터 0건 | 부정 0 + 부정 필터 | 경고, `fil_data` 비움 | §6.1 필터(중립) 오류 |
| EX-07 | Feature Envy | Filters→Constants 내부 | 단일 Registry 후 제거 | §4.2 |
| EX-08 | Lava Flow | FileHandler | 미사용 0커버 또는 삭제 | §4.2 |
| EX-09 | 부분 문자열 | `불만` vs `불만족` | `불만` hit | containsAny 정책 |
| EX-10 | 앵커 E2E | analyze→filter→download | CSV 1행 = 앵커 본문 | §2.2 사용자 시나리오 |

---

## 5. 커버리지 목표

### 5.1 레이어별 하한

| 레이어 | 태그 | 경로 | Line | Branch | 근거 |
|--------|------|------|------|--------|------|
| **Domain** | `[domain]` | `src/entity/`, `tests/domain/` | **≥ 95%** | **≥ 90%** | PRD AC-03, 분류 핵심 |
| **Boundary** | `[boundary]` | `src/boundary/`, `tests/boundary/` | **≥ 85%** | **≥ 80%** | Parser, Presenter, Router |
| **Data** | `[data]` | `tests/data/` | **≥ 90%** | **≥ 85%** | fixture 로더 |
| **Overall** | 합산 | — | **≥ 90%** | **≥ 85%** | project_purpose §6.1 **90% 이상** |

### 5.2 앵커가 닫아야 할 분기 (현행 `src/cpp`)

| 분기 | 파일 | 앵커 트리거 |
|------|------|-------------|
| 부정 키워드 | `Constants.cpp` / Registry | `"불만"` |
| 긍정 스킵 → 부정 | `TextAnalyzer::sent` | 긍정 없음 |
| 배송 main | `TextAnalyzer::kw` | `"배송"` |
| 필터 통과 | `Filters::fil` | 부정+배송 (목표=Converter) |

### 5.3 게이트

| 시점 | 조건 |
|------|------|
| 2단계 미션 완료 | Overall line ≥90%, Catch2 GREEN |
| Story 완료 | Domain line ≥95%, branch ≥90% |
| Epic 인수 | Boundary ≥85%, golden N≥20 불일치 0 |

---

## 6. gcov / lcov / `main.cpp` 측정 전략

### 6.1 원칙

| 원칙 | 설명 |
|------|------|
| 분리 빌드 | `build-cov` + `--coverage -O0 -g` |
| Domain 우선 | 비즈니스 분기는 **entity 95%+**; Overall 90%는 project_purpose 정렬 |
| main 분리 | God Function — Domain 합산 **제외**, Boundary 85%+ |

### 6.2 CMake (coverage 타깃)

```cmake
option(ENABLE_COVERAGE "gcov instrumentation" OFF)
if(ENABLE_COVERAGE)
  add_compile_options(--coverage -O0 -g)
  add_link_options(--coverage)
endif()

# 권장: 테스트 타깃 분리
# add_executable(feedback_analyzer_domain_tests ...)  # TextAnalyzer/Converter만
# add_executable(feedback_analyzer_boundary_tests ...) # main 라우트 Fake
```

```bash
cmake -S . -B build-cov -G "MinGW Makefiles" \
  -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe \
  -DENABLE_COVERAGE=ON
cmake --build build-cov
ctest --test-dir build-cov -R "domain|boundary" --output-on-failure
```

### 6.3 gcov 워크플로

```bash
ctest --test-dir build-cov

cd build-cov
# MinGW: .gcda → .gcov
gcov -b -c -o CMakeFiles/feedback_analyzer.dir/src/cpp \
  CMakeFiles/feedback_analyzer.dir/src/cpp/main.cpp.gcda

gcov -o CMakeFiles/feedback_analyzer_domain.dir ../src/entity/Converter.cpp
```

### 6.4 lcov — Domain / Boundary / main.cpp 분리

```bash
lcov --capture --directory build-cov --output-file coverage.info \
  --rc lcov_branch_coverage=1

lcov --extract coverage.info '*/src/entity/*'     -o coverage-domain.info
lcov --extract coverage.info '*/src/boundary/*'   -o coverage-boundary.info
lcov --extract coverage.info '*/src/cpp/main.cpp' -o coverage-main.info

genhtml coverage-domain.info   -o report-domain   --branch-coverage
genhtml coverage-boundary.info -o report-boundary --branch-coverage
genhtml coverage-main.info     -o report-main     --branch-coverage
```

### 6.5 `main.cpp` 전용 전략

| 항목 | 내용 |
|------|------|
| **측정 이유** | 라우팅·`renderPage`·세션·try/catch — project_purpose §5.1 분리 대상 |
| **테스트 유형** | `[boundary]` Mock `httplib::Request` / `[integration]` 실 HTTP |
| **앵커 라인** | `POST /analyze` L248–280 (`sent`/`kw`), `POST /filter`, `GET /download` |
| **목표** | line **≥85%**, branch **≥80%** (`coverage-main.info`) |
| **RED 표시** | 빈 text·`COLON_MISSING` 미구현 분기 → lcov **uncovered** → Story 연결 |
| **REFACTOR 후** | `main.cpp` 축소 → extract된 `Router`/`HtmlRenderer`로 lcov 경로 이동 |

### 6.6 gcovr 대안 (Windows)

```bash
gcovr --root .. --filter src/entity          --line-threshold 95 --branch-threshold 90
gcovr --root .. --filter src/boundary        --line-threshold 85
gcovr --root .. --filter src/cpp/main.cpp    --line-threshold 85
gcovr --root ..                              --line-threshold 90
```

### 6.7 로컬 게이트

```bash
lcov --summary coverage-domain.info   # lines ≥ 95%
lcov --summary coverage-boundary.info # lines ≥ 85%
lcov --summary coverage.info          # lines ≥ 90% (project_purpose)
```

---

## 7. 추적성 매트릭스

| 테스트 ID | Catch2 | Gherkin | project_purpose / AC |
|-----------|--------|---------|----------------------|
| DOM-001~002 | domain | GH-01, GH-03 | §2.1 분류·감정 |
| DOM-004 | domain | GH-08 | §2.2 필터·CSV |
| BND-P01 | boundary | 03_feedback INV-PARSE-02 | 형식 오류 `:` |
| BND-P09 | boundary/integration | — | 앵커 일반 입력 |
| INT-ANCHOR | integration | 06_readme | §2.2 E2E |

---

## 8. 리스크·RED

| 리스크 | 완화 |
|--------|------|
| purpose §2.3 `화가` vs 앵커 `불만` | BND-D02 vs BND-D01 대비 |
| 테스트 없음 (§4.1) | P0 Catch2 + ENABLE_COVERAGE |
| 필터 중립 버그 (§6.1) | BND-F05, DOM-004 |
| main God Function | lcov main 분리 + Extract Class 후 재측정 |

---

## 9. 변경 이력

| 버전 | 일자 | 변경 |
|------|------|------|
| 0.1 | 2026-05-22 | 초안 (루트 test_plan.md) |
| 1.0 | 2026-05-22 | `docs/` 이관, project_purpose.md §6.1·§2.3 정합 |

**다음 작업**: `tests/domain/` 앵커 RED → CMake Catch2 → `build-cov` + `report-domain`/`report-main` 생성.
