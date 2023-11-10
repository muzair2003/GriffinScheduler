import csv
import sys
import threading
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.service import Service
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import NoSuchElementException
from selenium.webdriver.chrome.options import Options
import time
import re

# Check if the course code argument is passed
if len(sys.argv) < 2:
    print("Usage: python script_name.py <course_code>")
    sys.exit(1)  # Exit the script if the course code is not provided

# Get the course code from the command line arguments
course_code = sys.argv[1]

# Setup Chrome options
chrome_options = Options()
chrome_options.add_argument("--headless")
chrome_options.add_argument("--no-sandbox")
chrome_options.add_argument("--disable-dev-shm-usage")
chrome_options.add_argument("--disable-gpu")
chrome_options.add_argument("--disable-extensions")
chrome_options.add_argument("--disable-popup-blocking")
chrome_options.add_argument("--window-size=1920,1080")
chrome_options.add_argument("--disable-images")
chrome_options.add_argument("--disable-javascript")
chrome_options.add_argument("--incognito")
chrome_options.add_argument("--disable-infobars")
chrome_options.add_argument("--disable-browser-side-navigation")


# Setup Selenium Chrome WebDriver with headless option
service = Service(ChromeDriverManager().install())
driver = webdriver.Chrome(service=service, options=chrome_options)

# Open the webpage
driver.get(f'https://colleague-ss.uoguelph.ca/Student/Courses/Search?keyword={course_code.replace(" ", "+")}')

try:
    # Define the ID for the clickable element based on the course code entered by the user
    clickable_element_id = f"collapsible-view-available-sections-for-{course_code.replace(' ', '*')}-groupHeading"

    # Wait for the button to be clickable, and click it
    WebDriverWait(driver, 10).until(
        EC.element_to_be_clickable((By.ID, clickable_element_id))
    ).click()

    # Wait until the required elements are loaded
    WebDriverWait(driver, 10).until(
        EC.presence_of_element_located((By.XPATH, "//*[contains(@id,'section-') and contains(@id,'-meeting-')]"))
    )

        # Open a new CSV file to write into
    with open(f'{course_code.replace(" ", "_")}.csv', mode='w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        # Write the header to the CSV file
        writer.writerow(['ID', 'StartTime', 'EndTime', 'Days', 'Type'])

        # Initialize an empty set to keep track of unique rows
        seen_rows = set()

        # Find all unique section IDs
        section_elements = driver.find_elements(By.XPATH, "//*[contains(@id,'section-') and contains(@id,'-meeting-')]")
        unique_ids = set(re.search(r'section-(\d+)-meeting', el.get_attribute('id')).group(1) for el in section_elements if re.search(r'section-(\d+)-meeting', el.get_attribute('id')))

        row_data=[]
        # For each unique ID, find all related details
        for section_id in unique_ids:
            # Handle up to 4 meeting-times (0 to 3)
            for i in range(4):
                try:
                    row_data = (
                    section_id,
                    driver.find_element(By.ID, f'section-{section_id}-meeting-times-start-{i}').text,
                    driver.find_element(By.ID, f'section-{section_id}-meeting-times-end-{i}').text,
                    driver.find_element(By.ID, f'section-{section_id}-meeting-days-{i}').text.replace('T', 'Tu'),
                    driver.find_element(By.ID, f'section-{section_id}-meeting-instructional-method-{i}').text
                    )
                    # If the type is 'EXAM', skip this row and continue with the next iteration
                    if row_data[-1] == "EXAM":
                        continue
                    # Check if the tuple is in the set of seen rows
                    if row_data in seen_rows:
                        # If it is, skip this row as it's a duplicate
                        continue

                    # If the row is new, add it to the set and write to the CSV
                    seen_rows.add(row_data)
                    writer.writerow(row_data)
                    #print(f"ID: {section_id}, Index: {i}, StartTime: {row_data[1]}, EndTime: {row_data[2]}, Days: {row_data[3]}, Type: {row_data[4]}")
                except NoSuchElementException:
                    # If the element does not exist, break the loop and continue with the next ID
                    break

finally:
    # Close the WebDriver
    driver.quit()
