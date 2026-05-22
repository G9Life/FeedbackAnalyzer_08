# language: ko
@level4 @epic @integrated
Feature: Feedback Analyzer 통합 분석 계약
  감정·카테고리 분류, 입력 검증, 필터, 통계·직렬화를 단일 Registry·Converter 기준으로 수행한다.

  Background:
    Given UnitRegistry가 기본 ratios 스냅샷으로 초기화되어 있다
    And 감정 Hub는 "중립"이다
    And 감정 우선순위는 "긍정" 다음 "부정" 다음 "중립"이다
    And 카테고리 Unit "배송","품질","가격","서비스","사용성"이 main 키워드 그룹을 가진다

  Rule: 피드백당 감정은 정확히 하나
  Rule: 키워드 미매칭 시 Hub 중립
  Rule: main 키워드 매칭 시에만 카테고리 건수 증가

  @GH-01 @INV-D2 @INV-D3
  Scenario Outline: 키워드 기반 감정 분류
    Given 피드백 본문이 "<text>"이다
    When Converter가 감정을 분류한다
    Then 분류된 감정 id는 "<sentiment>"이다

    Examples:
      | text                           | sentiment |
      | 감사합니다. 정말 만족합니다.   | 긍정      |
      | 배송이 너무 늦어요. 화가 납니다. | 부정    |
      | 전반적으로 무난했습니다.       | 중립      |

  @GH-02 @INV-D5
  Scenario: 다건 감정 집계 합 보존
    Given 피드백 3건이 분석 대기 상태이다
    When Converter가 감정 집계를 수행한다
    Then 긍정·부정·중립 건수 합은 3이다

  @GH-03 @INV-D7
  Scenario Outline: main 키워드 카테고리 탐지
    Given 피드백 본문이 "<text>"이다
    When Converter가 카테고리 집계를 수행한다
    Then "<category>" 건수는 1 이상이다

    Examples:
      | text                | category |
      | 택배가 늦었습니다   | 배송     |
      | 품질이 최악입니다   | 품질     |
      | 가격이 비쌉니다     | 가격     |
      | 상담이 불친절합니다 | 서비스   |
      | 설명서가 어렵습니다 | 사용성   |

  @GH-04 @INV-D7
  Scenario: 복수 카테고리 동시 매칭
    Given 피드백 본문이 "배송은 빠른데 가격이 비싸요"이다
    When Converter가 카테고리 집계를 수행한다
    Then "배송" 건수는 1 이상이다
    And "가격" 건수는 1 이상이다

  @GH-05 @INV-ERR-01
  Scenario: 알 수 없는 감정 라벨
    When Domain 파서가 "화남:짜증납니다"를 파싱한다
    Then 오류 code는 "UNKNOWN_EMOTION"이다

  @GH-06 @INV-PARSE-01
  Scenario Outline: 빈 입력 거부
    When 사용자가 "/analyze"에 본문 "<text>"를 POST한다
    Then 오류 code는 "EMPTY_TEXT"이다

    Examples:
      | text |
      |      |
      |  \t  |

  @GH-07
  Scenario: CSV text 열 필수
    When 사용자가 header가 "foo"인 CSV를 "/upload"한다
    Then 오류 code는 "CSV_MISSING_TEXT_COLUMN"이다

  @GH-08 @INV-D6
  Scenario: 감정 부정 AND 카테고리 배송
    Given 세션에 부정·배송 피드백 1건과 기타 피드백이 있다
    When 사용자가 "/filter"에 감정 "부정" 카테고리 "배송"을 POST한다
    Then 필터 결과 모든 항목의 감정 id는 "부정"이다
    And 필터 결과 건수는 1이다

  @GH-09 @INV-D5 @INV-D6
  Scenario: 필터 후 집계는 부분집합만
    When 사용자가 "/filter"에 감정 "긍정" 카테고리 "전체"를 POST한다
    Then 감정 통계 카드 긍정 건수 합은 필터 결과 건수와 같다

  @GH-10 @INV-VIZ-01
  Scenario: HTML 감정·키워드 통계 카드
    When 사용자가 "/"를 GET한다
    Then "감정 분포" 섹션의 건수 합은 표시 피드백 건수와 같다

  @GH-11 @INV-OUT-JSON-01
  Scenario: JSON sentimentStats 스키마
    When Presenter가 JSON으로 직렬화한다
    Then "sentimentStats" 배열의 각 항목에 id와 count가 있다

  @GH-12 @INV-OUT-CSV-01
  Scenario: CSV 다운로드
    When 사용자가 "/download"를 GET한다
    Then CSV 헤더는 "text"이다
    And 데이터 행 수는 fil_data 건수와 같다

  @GH-13 @INV-OUT-TBL-02
  Scenario: HTML 표 이스케이프
    Given 피드백 본문에 "<script>"가 포함된다
    When Presenter가 HTML 테이블을 렌더링한다
    Then 셀에 이스케이프되지 않은 "<script>"가 없다

  @README-01
  Scenario: Dashboard first visit
    When the user sends GET "/"
    Then the page should display input and upload forms

  @README-02
  Scenario: Valid CSV three rows
    When the user uploads CSV with header "text" and 3 data rows
    Then the session feedback count should be 3

  @README-03
  Scenario: Manual input visualization
    When the user posts feedback to "/analyze"
    Then sentiment and keyword statistics should be visible

  @README-04
  Scenario: Filter negative and delivery then download CSV
    When the user filters by "부정" and "배송" then GET "/download"
    Then CSV data row count should match filtered count
