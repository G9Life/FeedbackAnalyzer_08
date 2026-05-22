# language: ko
@level4 @entity @sentiment
Feature: 피드백 감정 분류
  고객 피드백 문장을 규칙 기반으로 긍정·부정·중립 중 하나의 감정으로 분류한다.
  집계·필터·단건 분류는 동일한 Registry 규칙을 사용해야 한다.

  Background:
    Given UnitRegistry가 기본 ratios 스냅샷으로 초기화되어 있다
    And 감정 Hub는 "중립"이다
    And 감정 우선순위는 "긍정" 다음 "부정" 다음 "중립"이다

  Rule: 피드백당 감정은 정확히 하나
  Rule: 키워드 미매칭 시 Hub 중립으로 귀속

  @INV-D2 @INV-D3
  Scenario Outline: 키워드 기반 감정 분류
    Given 피드백 본문이 "<text>"이다
    When Converter가 감정을 분류한다
    Then 분류된 감정 id는 "<sentiment>"이다
    And 결과 집합은 {긍정, 부정, 중립} 중 하나뿐이다

    Examples:
      | text                         | sentiment |
      | 감사합니다. 정말 만족합니다. | 긍정      |
      | 배송이 너무 늦어요. 화가 납니다. | 부정   |
      | 전반적으로 무난했습니다.     | 중립      |
      | 상품은 괜찮은데 포장이 별로예요. | 부정   |

  @INV-D1
  Scenario: 동일 문장 재분류 결과 일치
    Given 피드백 본문이 "친절하고 빠른 배송 감사합니다"이다
    When Converter가 감정을 2회 연속 분류한다
    Then 1회째 감정 id와 2회째 감정 id가 같다

  @INV-D3
  Scenario Outline: 긍정·부정 키워드 공존 시 우선순위 적용
    Given 피드백 본문이 "<text>"이다
    When Converter가 감정을 분류한다
    Then 분류된 감정 id는 "<expected>"이다

    Examples:
      | text                              | expected |
      | 좋은데 아쉬워요 만족은 하는데 별로 | 긍정     |
      | 최악이에요 다시는 안 삽니다       | 부정     |

  @INV-D5
  Scenario: 다건 집계 건수 합 보존
    Given 다음 피드백 목록이 있다
      | text           |
      | 최고입니다     |
      | 별로예요       |
      | 그냥 그래요    |
    When Converter가 감정 집계를 수행한다
    Then 긍정 건수와 부정 건수와 중립 건수의 합은 3이다
