import csv
import sys
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
chrome_options.add_argument("--headless")  # Ensure GUI is off
chrome_options.add_argument("--no-sandbox")  # Bypass OS security model
chrome_options.add_argument("--disable-dev-shm-usage")  # Overcome limited resource problems

# Setup Selenium Chrome WebDriver with headless option
service = Service(ChromeDriverManager().install())
driver = webdriver.Chrome(service=service, options=chrome_options)

# Format the course code to fit the URL structure (replace spaces with '+')
formatted_course_code = course_code.replace(" ", "+")

# Construct the URL with the user's input
url = f'https://colleague-ss.uoguelph.ca/Student/Courses/Search?keyword={formatted_course_code}'

# Open the webpage
driver.get(url)

try:
    # Define the ID for the clickable element based on the course code entered by the user
    clickable_element_id = f"collapsible-view-available-sections-for-{course_code.replace(' ', '*')}-groupHeading"

    # Wait for the button to be clickable, and click it to expand the sections
    WebDriverWait(driver, 10).until(
        EC.element_to_be_clickable((By.ID, clickable_element_id))
    ).click()

    # Wait for a moment to ensure the content has loaded
    time.sleep(5)  # Replace with a more robust wait condition if necessary

        # Open a new CSV file to write into
    with open('course_sections.csv', mode='w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        # Write the header to the CSV file
        writer.writerow(['ID', 'StartTime', 'EndTime', 'Days', 'Type'])

        # Initialize an empty set to keep track of unique rows
        seen_rows = set()

        # Find all unique section IDs
        section_elements = driver.find_elements(By.XPATH, "//*[contains(@id,'section-') and contains(@id,'-meeting-')]")
        unique_ids = set(re.search(r'section-(\d+)-meeting', el.get_attribute('id')).group(1) for el in section_elements if re.search(r'section-(\d+)-meeting', el.get_attribute('id')))

        # For each unique ID, find all related details
        for section_id in unique_ids:
            # Handle up to 4 meeting-times (0 to 3)
            for i in range(4):
                try:
                    start_time_element = driver.find_element(By.ID, f'section-{section_id}-meeting-times-start-{i}').text
                    end_time_element = driver.find_element(By.ID, f'section-{section_id}-meeting-times-end-{i}').text
                    days_element = driver.find_element(By.ID, f'section-{section_id}-meeting-days-{i}').text
                    type_element = driver.find_element(By.ID, f'section-{section_id}-meeting-instructional-method-{i}').text

                    # If the type is 'EXAM', skip this row and continue with the next iteration
                    if type_element == "EXAM":
                        continue

                    # Create a tuple of the row data
                    row_data = (section_id, start_time_element, end_time_element, days_element, type_element)

                    # Check if the tuple is in the set of seen rows
                    if row_data in seen_rows:
                        # If it is, skip this row as it's a duplicate
                        continue

                    # If the row is new, add it to the set and write to the CSV
                    seen_rows.add(row_data)
                    writer.writerow(row_data)
                    print(f"ID: {section_id}, Index: {i}, StartTime: {start_time_element}, EndTime: {end_time_element}, Days: {days_element}, Type: {type_element}")
                except NoSuchElementException:
                    # If the element does not exist, break the loop and continue with the next ID
                    break

finally:
    # Close the WebDriver
    driver.quit()
