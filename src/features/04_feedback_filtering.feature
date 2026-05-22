# language: ko
@level4 @control @filter
Feature: 감정·카테고리 조합 필터링
  세션 피드백에서 감정 필터와 카테고리 필터를 조합해 부분집합을 만든다.
  필터 결과의 모든 항목은 선택한 감정·카테고리 조건을 만족해야 한다.

  Background:
    Given UnitRegistry가 기본 ratios 스냅샷으로 초기화되어 있다
    And 세션에 다음 피드백이 있다
      | text                              |
      | 배송이 늦어서 최악이에요          |
      | 가격은 비싸지만 만족합니다        |
      | 택배 빠르고 친절해요              |
      | 설명서가 어려워요                 |
    And 사용자가 "/analyze"로 집계를 완료했다

  Rule: 감정 필터 "전체"는 감정 조건을 적용하지 않음
  Rule: 필터 후 집계는 부분집합만 대상

  @INV-D6
  Scenario Outline: 감정 필터 정합성
    When 사용자가 "/filter"에 감정 "<sentiment>" 카테고리 "전체"를 POST한다
    Then HTTP 상태 코드는 200이다
    And 필터 결과 건수는 <count>이다
    And 필터 결과 모든 항목의 감정 id는 "<sentiment>"이다

    Examples:
      | sentiment | count |
      | 부정      | 1     |
      | 긍정      | 2     |
      | 전체      | 4     |

  @INV-D6 @INV-D7
  Scenario: 감정 부정 AND 카테고리 배송 조합
    When 사용자가 "/filter"에 감정 "부정" 카테고리 "배송"을 POST한다
    Then 필터 결과 건수는 1이다
    And 유일한 결과 본문에 "배송" 관련 표현이 포함된다
    And 해당 항목 감정 id는 "부정"이다

  @INV-D5 @INV-D6
  Scenario: 필터 후 감정 집계 합 = 결과 건수
    When 사용자가 "/filter"에 감정 "긍정" 카테고리 "전체"를 POST한다
    Then 필터 결과 건수는 2이다
    And 감정 통계 카드 긍정 건수의 합은 2이다

  @INV-ERR-01
  Scenario: 미등록 카테고리 필터 오류
    When 사용자가 "/filter"에 감정 "전체" 카테고리 "해외배송"을 POST한다
    Then HTTP 상태 코드는 400이다
    And 오류 code는 "UNKNOWN_CATEGORY_FILTER"이다

  Scenario: 필터 결과 없음 경고
    When 사용자가 "/filter"에 감정 "중립" 카테고리 "전체"를 POST한다
    Then HTTP 상태 코드는 200이다
    And 경고 메시지에 "필터링 결과가 없습니다"가 표시된다
    And 감정 통계 카드가 비어 있지 않고 0건으로 표시된다
