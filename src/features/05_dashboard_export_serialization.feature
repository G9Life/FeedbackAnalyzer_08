# language: ko
@level4 @boundary @presenter @export
Feature: 분석 대시보드 통계 및 결과 직렬화
  감정·카테고리 분포 통계 카드와 피드백 목록을 HTML·JSON·CSV로 직렬화한다.
  필터 적용 후 출력은 부분집합 기준이며 XSS 이스케이프를 적용한다.

  Background:
    Given UnitRegistry가 기본 ratios 스냅샷으로 초기화되어 있다
    And 세션에 피드백 3건이 분석 완료 상태이다
      | text                    | sentiment |
      | 최고의 서비스 감사합니다 | 긍정      |
      | 배송 지연 불만입니다    | 부정      |
      | 그냥 그랬어요           | 중립      |
    And 사용자가 "/filter"에 감정 "부정" 카테고리 "전체"로 필터했다
    And fil_data에 필터 결과가 저장되었다

  Rule: 통계 카드 감정 건수 합 = 표시 대상 피드백 건수
  Rule: CSV 헤더는 text 단일 열

  @INV-D5 @INV-VIZ-01
  Scenario: HTML 감정·카테고리 통계 카드
    When 사용자가 "/" 대시보드를 GET한다
    Then HTTP 상태 코드는 200이다
    And "감정 분포" 섹션에 긍정·부정·중립 건수가 표시된다
    And 표시된 감정 건수 합은 현재 표시 피드백 건수와 같다
    And "키워드 분석" 섹션에 등록 카테고리 id만 표시된다

  @INV-OUT-JSON-01
  Scenario Outline: JSON 직렬화 스키마
    When Presenter가 분석 스냅샷을 JSON으로 직렬화한다
    Then JSON 경로 "<path>"가 존재한다
    And "<path>" 각 항목은 id와 count 필드를 가진다

    Examples:
      | path              |
      | sentimentStats    |
      | categoryStats     |
      | feedbacks         |

  @INV-OUT-JSON-02
  Scenario: JSON 본문 특수문자 이스케이프
    Given 피드백 본문이 "좋아요 \"인용\" <script>alert(1)</script>"이다
    When Presenter가 JSON으로 직렬화한다
    Then JSON 문자열에 원시 "<script>" 태그가 없다
    And 이스케이프된 따옴표가 포함된다

  @INV-OUT-CSV-01 @INV-D6
  Scenario Outline: CSV 다운로드 계약
    When 사용자가 "/download"를 GET한다
    Then HTTP 상태 코드는 <status>이다
    And Content-Type에 "text/csv"가 포함된다
    And CSV 헤더 첫 줄은 "text"이다
    And 데이터 행 수는 <rows>이다

    Examples:
      | status | rows |
      | 200    | 1    |

  @INV-OUT-CSV-02
  Scenario: CSV UTF-8 한글 보존
    When 사용자가 "/download"를 GET한다
    Then 응답 본문에 "배송 지연 불만입니다"가 깨지지 않고 포함된다
    And BOM이 선택적으로 포함될 수 있다

  @INV-OUT-TBL-01 @INV-OUT-TBL-02
  Scenario Outline: HTML 테이블 행·이스케이프
    When Presenter가 피드백 목록을 HTML 테이블로 렌더링한다
    Then 테이블 데이터 행 수는 <rows>이다
    And 셀 내용에 이스케이프되지 않은 "<script>"가 없다

    Examples:
      | rows |
      | 1    |

  @INV-ERR-02
  Scenario: 다운로드할 데이터 없음
    Given fil_data가 비어 있다
    When 사용자가 "/download"를 GET한다
    Then HTTP 상태 코드는 404이다
    And 오류 code는 "NO_DATA_TO_EXPORT"이다
