# language: ko
@level4 @entity @category
Feature: 피드백 카테고리 키워드 분류
  피드백 본문에서 배송·품질·가격·서비스·사용성 카테고리를 main 키워드 그룹으로 탐지한다.
  서브 키워드만 있는 경우 카테고리로 집계하지 않는다.

  Background:
    Given UnitRegistry에 카테고리 Unit이 등록되어 있다
      | category |
      | 배송     |
      | 품질     |
      | 가격     |
      | 서비스   |
      | 사용성   |
    And 각 카테고리는 main 키워드 그룹을 가진다

  Rule: main 키워드 매칭 시에만 카테고리 건수 증가
  Rule: 한 피드백이 여러 카테고리에 매칭되면 각각 1회씩 증가

  @INV-D7
  Scenario Outline: main 키워드로 카테고리 탐지
    Given 피드백 본문이 "<text>"이다
    When Converter가 카테고리 집계를 수행한다
    Then "<category>" 건수는 1 이상이다
    And 다른 등록 카테고리 건수는 변경되지 않았거나 0이다

    Examples:
      | text                           | category |
      | 택배가 3일 늦게 도착했습니다   | 배송     |
      | 재질이 싸구려 같고 고장났어요  | 품질     |
      | 가격이 너무 비싸요             | 가격     |
      | 상담원이 불친절했습니다        | 서비스   |
      | 설명서가 어려워요              | 사용성   |

  @INV-D7
  Scenario: 서브 키워드만 있을 때 main 미매칭이면 미집계
    Given 피드백 본문이 "배송지연만 언급된 내부 코드 문자열"이고 main 키워드는 매칭되지 않는다
    When Converter가 카테고리 집계를 수행한다
    Then "배송" 건수는 0이다

  Scenario: 복수 카테고리 동시 매칭
    Given 피드백 본문이 "배송은 빠른데 가격이 비싸고 품질도 별로예요"이다
    When Converter가 카테고리 집계를 수행한다
    Then "배송" 건수는 1 이상이다
    And "가격" 건수는 1 이상이다
    And "품질" 건수는 1 이상이다
