# language: ko
@level4 @readme @boundary
Feature: README premises — feedback collection, classification, visualization, export
  As a user of the Feedback Analyzer web app
  I want the flows described in README (access, input, filter, download)
  So that feedback is collected, classified, visualized, and exported under a stable contract.

  Background:
    Given the Feedback Analyzer server is running at "http://localhost:8080"
    And the session feedback list is empty

  @README-01
  Scenario: Dashboard first visit shows input and upload forms
    When the user sends GET "/" 
    Then the response status code should be 200
    And the response Content-Type should include "text/html"
    And the page should display a text feedback form posting to "/analyze"
    And the page should display a CSV upload form posting to "/upload"
    And the page should display a filter form posting to "/filter"

  @README-02
  Scenario: Valid CSV bulk upload loads three feedback rows
    Given the session feedback list is empty
    When the user uploads a CSV file to "/upload" with content:
      """
      text
      배송이 빨라서 만족합니다
      품질이 별로예요 환불 원합니다
      가격은 괜찮아요
      """
    Then the response status code should be 200
    And the session feedback count should be 3
    And the first CSV row should be treated as header only
    And each loaded feedback body should be non-empty

  @README-03 @INV-VIZ-01
  Scenario: Manual text input shows sentiment and keyword visualization
    Given the session feedback list is empty
    When the user posts feedback text "친절한 서비스에 감사합니다" to "/analyze"
    Then the response status code should be 200
    And the page should show a "감정 분포" sentiment statistics section
    And the page should show a keyword or category statistics section
    And the positive sentiment count should be at least 1
    And the service category count should be at least 1
    And the sum of sentiment counts should equal the session feedback count

  @README-04 @INV-D6 @INV-OUT-CSV-01
  Scenario: Filter by negative sentiment and delivery category then download CSV
    Given the session contains analyzed feedback:
      | text                         |
      | 배송이 늦어서 불만입니다     |
      | 택배 빠르고 좋아요           |
      | 가격이 비싸지만 만족합니다   |
    When the user posts sentiment filter "부정" and keyword filter "배송" to "/filter"
    Then the response status code should be 200
    And the filtered result count should be 1
    And every filtered item should have sentiment id "부정"
    When the user sends GET "/download"
    Then the response status code should be 200
    And the Content-Disposition should include "filtered_feedback.csv"
    And the CSV header line should be "text"
    And the CSV data row count should be 1
    And the CSV data row should contain delivery-related feedback text

  @README-05
  Scenario: Keyword-based classification appears after analysis
    Given the session contains feedback "택배가 파손되어 배송이 최악이었습니다"
    When the user posts the same session state to "/analyze" with no new text
    Then the response status code should be 200
    And the delivery category count should be at least 1
    And the page should list registered categories including "배송"

  @README-06
  Scenario: Sentiment analysis shows positive, negative, and neutral buckets
    Given the session contains feedback:
      | text                     |
      | 정말 만족합니다 최고예요 |
      | 환불하고 싶어요 최악     |
      | 그냥 무난했습니다        |
    When the user triggers analysis on "/analyze"
    Then the sentiment statistics should include labels "긍정", "부정", and "중립"
    And the sum of positive, negative, and neutral counts should be 3

  @README-07
  Scenario: Filter and search by keyword category only
    Given the session contains analyzed feedback:
      | text                           |
      | 배송이 늦어서 불만입니다       |
      | 상담원이 불친절했습니다        |
      | 가격이 비싸요                  |
    When the user posts sentiment filter "전체" and keyword filter "서비스" to "/filter"
    Then the response status code should be 200
    And the filtered result count should be 1
    And the filtered item text should relate to customer service

  @README-08
  Scenario: CSV without required text column is rejected
    When the user uploads a CSV file to "/upload" with content:
      """
      foo,bar
      a,b
      """
    Then the response status code should be 422
    And the error code should be "CSV_MISSING_TEXT_COLUMN"
    And the session feedback count should remain 0
