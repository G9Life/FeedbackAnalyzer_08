# 결함 목록 (Defect List)

| 항목 | 내용 |
|------|------|
| **문서 ID** | DEF-2026-05-22 |
| **기준일** | 2026-05-22 |
| **앵커 샘플** | `배송이 너무 늦어요. 불만입니다.` |
| **근거** | Catch2 30건 RED 분석, [test_plan.md](./test_plan.md), 레거시 `src/cpp` 대조 |
| **ctest (스텁)** | `tests/red_phase_stubs.cpp` 수정 후 **30/30 PASS** |
| **레거시 앱** | `feedback_analyzer.exe` — 아래 **Open** 항목 잔존 |

---

## 심각도 정의

| 등급 | 기준 |
|------|------|
| **Critical** | 분류·집계·필터·입력 검증 결과가 계약과 **완전히 다름** |
| **Major** | 경계 문장·필터·설정·동적 등록에서 **GH/PRD와 불일치** |
| **Minor** | 출력 포맷·HTTP 오류 표현·로그 수준 불일치 |
| **Info** | 코드 스멜·문서·테스트 인프라 (기능 오류 아님) |

**변환 타입**: `classify` · `aggregate` · `filter` · `parse` · `loadConfig` · `registerUnit` · `http`

---

## 1. RED 스텁 결함 (Catch2 — 초기 RED 26/30 실패)

> **위치**: `tests/red_phase_stubs.cpp` (초기 커밋)  
> **상태**: 스텁 최소 구현으로 **Closed** (ctest GREEN). **프로덕션 `src/entity` 미구현** → 레거시와 동일 결함 재발 가능.

| ID | Severity | 변환 타입 | 재현 절차 | 기대값 | 실제값 | 근본 원인 | 수정 요약 |
|----|----------|-----------|-----------|--------|--------|-----------|-----------|
| DEF-001 | Critical | classify | `ctest -R test_classify_anchor` | `sentimentId` = **부정** | **중립** (항상 Hub) | L44-46 `classify()` no-op | 긍정→부정→중립 if-else 키워드 매칭 (`불만`∈부정) |
| DEF-002 | Critical | aggregate | `ctest -R test_aggregate_anchor` | 부정=1, 배송=1 | 빈 map / key 없음 | L48-50 `aggregate()` `{}` | 피드백별 `classify`+`categoryHits` 집계 |
| DEF-003 | Critical | filter | `ctest -R test_filter_negative_shipping` | 필터 1건(앵커) | size=0 | L52-56 `filter()` 빈 벡터 | `classify` 결과로 sentiment·category AND |
| DEF-004 | Critical | parse | `ctest -R test_parse_labelbody_colon_missing` | `COLON_MISSING`, 실패 | success=true | L70-72 `parseLabelBody()` 무검증 | `:` 없으면 400·field=text |
| DEF-005 | Critical | parse | `ctest -R test_parse_plain_empty` | `EMPTY_TEXT` | (통과) 또는 무응답 | L63-67 빈 문자열만 일부 처리 | trim 후 빈 본문 거부 |
| DEF-006 | Critical | filter | `ctest -R test_filter_unknown_category` | `UNKNOWN_CATEGORY_FILTER` | ok=true | L76-79 `validate()` 항상 Ok | 허용 enum 외 sentiment/category 거부 |
| DEF-007 | Major | loadConfig | `ctest -R test_config_yaml_valid` | success, Registry 3+5 | `CONFIG_PARSE_ERROR` | L86-88 `loadRatios()` 항상 실패 | fixture 존재·hub 검사 후 default 로드 |
| DEF-008 | Major | registerUnit | `ctest -R test_registerUnit_sentiment` | `sentimentId` = **만족** | **긍정** (기본 만족 키워드 선점) | L16-20 등록 상태 미반영 | `dynamicSentiment` 우선 매칭 후 기본 우선순위 |

---

## 2. 레거시 앱 결함 (`src/cpp` — Open)

> **재현**: `build\feedback_analyzer.exe` → `http://localhost:8080`  
> **상태**: **Open** (Catch2 스텁과 분리)

| ID | Severity | 변환 타입 | 재현 절차 | 기대값 | 실제값 | 근본 원인 | 수정 요약 |
|----|----------|-----------|-----------|--------|--------|-----------|-----------|
| DEF-L01 | Critical | classify | POST analyze `배송이 너무 늦어요. 화가 납니다.` | GH-01: **부정** | **중립** (집계) | `Constants.cpp` `화남`만 등록, 본문 `화가` 불일치 · `TextAnalyzer.h:29-34` | 키워드 정합 또는 GH/레거시 계약 통일 |
| DEF-L02 | Critical | filter | analyze 앵커 후 filter 감정=부정 | 1건 통과 | 집계·필터 규칙 불일치 시 0건 | `TextAnalyzer` vs `Filters::S_KEYWORDS` 이중 정의 · `Filters.h:33-38` | 단일 Registry·Converter 경로 (INV-D1) |
| DEF-L03 | Major | aggregate | `kw()` vs `fil()` 카테고리 | main만 집계 | 필터는 서브그룹 검사 | `TextAnalyzer::kw` main only · `Filters.h:50+` 서브 | `Converter::filter` = classify 기준 |
| DEF-L04 | Major | http | POST analyze `text=` (빈) | `EMPTY_TEXT` JSON/HTML | 200, 세션 변화 없음(조용히 스킵) | `main.cpp:254-261` 빈 text push 안 함 | Validator 거부 + 명시 오류 |
| DEF-L05 | Major | parse | `긍정:만족` / `만족합니다` 라벨 모드 | `COLON_MISSING` / 파싱 | 미구현(plain만) | `main.cpp:252` form text만 | `FeedbackParser` Boundary 분리 |
| DEF-L06 | Major | loadConfig | `loadRatios` / YAML | Registry 교체 | **미구현** | README DR-03 목표만 존재 | `src/data/ConfigLoader` + entity |
| DEF-L07 | Major | registerUnit | 런타임 Unit 추가 | OCP 반영 | **미구현** | `Constants::init()` 정적 | `UnitRegistry::registerUnit` |
| DEF-L08 | Minor | http | 필터 0건 | `error.code` 또는 구조화 응답 | HTML 경고만 | `main.cpp:338-341` | Presenter·ErrorMapper |
| DEF-L09 | Minor | http | CSV `text` 헤더 없음 | `CSV_MISSING_TEXT_COLUMN` 422 | 업로드 성공·무시 | `main.cpp:299-307` 헤더 미검증 | 첫 행 `text` 필수 검사 |
| DEF-L10 | Info | — | `Filters`·`TextAnalyzer` | DRY | `containsAny` 중복 | `Filters.h:13`, `TextAnalyzer.h:13` | 공통 유틸 또는 Registry |

---

## 3. Track A/B 테스트 매핑

| 테스트 ID | 결함 ID | 비고 |
|-----------|---------|------|
| TC-B-01 | DEF-001, DEF-L02 | 앵커 부정 |
| TC-B-02 | DEF-002 | aggregate |
| TC-B-03 | DEF-002 | main only |
| TC-B-04 | DEF-003, DEF-L02 | filter 일치 |
| TC-B-05 | DEF-001 | 우선순위 긍정 |
| TC-B-06 | DEF-001, DEF-L01 | 화가 vs 화남 |
| TC-B-07 | DEF-002 | 합=3 |
| TC-A-01 | DEF-005, DEF-L04 | EMPTY_TEXT |
| TC-A-02 | DEF-004, DEF-L05 | COLON_MISSING |
| TC-A-03 | DEF-004 | UNKNOWN_EMOTION |
| TC-A-04 | DEF-L09 | CSV 헤더 |
| TC-A-05 | DEF-L04 | trim (스텁/일부) |
| TC-A-06 | DEF-003, DEF-L02 | 부정+배송 |
| TC-A-07 | DEF-L08 | CSV/download |
| COV-* | DEF-007 | loadConfig |
| registerUnit | DEF-008, DEF-L07 | 동적 등록 |

---

## 4. 수정·검증 상태

| 구분 | Closed | Open |
|------|--------|------|
| Catch2 스텁 (`red_phase_stubs.cpp`) | DEF-001 ~ DEF-008 | — |
| `src/entity` + BCE 목표 | — | DEF-L01 ~ DEF-L07 (기능 이전 필요) |
| Boundary HTTP 계약 | — | DEF-L04, L05, L08, L09 |
| 코드 품질 | — | DEF-L10 (Info) |

### 회귀 확인 명령

```powershell
cd C:\DEV\FeedbackAnalyzer_08
cmake --build build
ctest --test-dir build
# 기대(현재): 100% tests passed, 0 failed out of 30
```

레거시 Open 항목 해소 후:

```powershell
.\build\feedback_analyzer.exe
# 수동: 앵커 analyze → 부정+배송 filter → CSV 1행
```

---

## 5. 변경 이력

| 버전 | 일자 | 변경 |
|------|------|------|
| 1.0 | 2026-05-22 | 초기 등록 — RED 26건 분석 + 레거시 10건 |
