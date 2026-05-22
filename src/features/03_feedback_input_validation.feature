# language: ko
@level4 @boundary @input
Feature: 피드백 입력 검증 및 파싱
  텍스트 입력·라벨:본문 형식·CSV 업로드를 검증하고 유효한 Feedback만 세션에 적재한다.
  실패 시 명시적 error.code와 field를 반환한다.

  Background:
    Given Feedback Analyzer 서버가 "http://localhost:8080"에서 실행 중이다
    And 세션 피드백 목록이 비어 있다

  Rule: 빈 본문과 공백만 있는 입력은 거부
  Rule: CSV는 text 열이 필수

  @INV-PARSE-01 @INV-ERR-02
  Scenario Outline: 단건 텍스트 입력 거부
    When 사용자가 "/analyze"에 본문 "<text>"를 POST한다
    Then HTTP 상태 코드는 <status>이다
    And 오류 code는 "<error_code>"이다
    And 오류 field는 "<field>"이다
    And 세션 피드백 건수는 0이다

    Examples:
      | text        | status | error_code  | field |
      |             | 400    | EMPTY_TEXT  | text  |
      |    \t\n     | 400    | EMPTY_TEXT  | text  |

  @INV-PARSE-02
  Scenario Outline: 라벨 본문 형식 파싱
    When Domain 파서가 입력 "<raw>"를 파싱한다
    Then 파싱 결과는 <result>이다

    Examples:
      | raw              | result        |
      | 긍정:만족합니다  | 성공          |
      | 만족합니다       | 실패 COLON_MISSING |
      | 부정:            | 실패 EMPTY_BODY    |
      | :본문만          | 실패 EMPTY_LABEL   |

  @INV-ERR-01
  Scenario: 알 수 없는 감정 라벨 거부
    When Domain 파서가 입력 "화남:짜증납니다"를 파싱한다
    Then 파싱은 실패한다
    And 오류 code는 "UNKNOWN_EMOTION"이다
    And 오류 message에 "화남"이 포함된다

  @INV-PARSE-04
  Scenario: 유효 텍스트 trim 후 적재
    When 사용자가 "/analyze"에 본문 "  배송이 빨라서 좋아요  "를 POST한다
    Then HTTP 상태 코드는 200이다
    And 세션 피드백 건수는 1이다
    And 저장된 본문 앞뒤 공백이 제거되었다

  @INV-ERR-02
  Scenario Outline: CSV 업로드 계약 위반
    When 사용자가 "/upload"에 다음 CSV 파일을 업로드한다
      """
      <csv_content>
      """
    Then HTTP 상태 코드는 <status>이다
    And 오류 code는 "<error_code>"이다

    Examples:
      | csv_content        | status | error_code                  |
      | foo,bar\na,b       | 422    | CSV_MISSING_TEXT_COLUMN     |
      | text\n\n           | 200    |                             |
