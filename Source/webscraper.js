const { chromium } = require('playwright');
const fs = require('fs');
const path = require('path');

(async () => {
  const browser = await chromium.launch({ headless: false });
  const page = await browser.newPage();
  const courseCode = 'ENGG 3100'; // Replace with dynamic course code

  await page.goto(`https://colleague-ss.uoguelph.ca/Student/Courses/Search?keyword=${courseCode.replace(" ", "+")}`);
  await page.waitForLoadState('domcontentloaded');

  // Find elements with 'id' attribute and check if it contains the specific strings
  const elements = await page.$$('[id]');
  for (const element of elements) {
    const id = await element.getAttribute('id');
    if (id && id.includes('collapsible-view-available-sections-for-ENGG') && id.includes('3100-groupHeading')) {
      await element.click();
      break;
    }
  }

  await page.waitForSelector("//*[contains(@id,'section-') and contains(@id,'-meeting-')]");

  const sectionElements = await page.$$("[id^='section-'][id*='-meeting-']");
  const uniqueIds = new Set();

  for (const element of sectionElements) {
    const id = await element.getAttribute('id');
    const match = id.match(/section-(\d+)-meeting/);
    if (match) {
      uniqueIds.add(match[1]);
    }
  }

  const uniqueRows = new Set();
  const data = [];

  for (const sectionId of uniqueIds) {
    const sectionText = await page.textContent(`#section-${sectionId}`);
    const sectionCodeMatch = sectionText.match(/(?<=\*)(\d+)/);
    const sectionCode = sectionCodeMatch ? sectionCodeMatch[0] : '';

    for (let i = 0; i < 4; i++) {
      try {
        const startTimeSelector = `#section-${sectionId}-meeting-times-start-${i}`;
        const endTimeSelector = `#section-${sectionId}-meeting-times-end-${i}`;
        const daysSelector = `#section-${sectionId}-meeting-days-${i}`;
        const methodSelector = `#section-${sectionId}-meeting-instructional-method-${i}`;

        if (await page.isVisible(startTimeSelector)) {
          const startTimeText = await page.textContent(startTimeSelector);
          const endTimeText = await page.textContent(endTimeSelector);
          const daysText = await page.textContent(daysSelector);
          const methodText = await page.textContent(methodSelector);

          if (methodText.trim() === "EXAM") {
            continue;
          }

          const key = `${sectionId}-${startTimeText}-${endTimeText}-${daysText}-${methodText}`;

          if (!uniqueRows.has(key)) {
            uniqueRows.add(key);
            data.push([sectionId, sectionCode, startTimeText, endTimeText, daysText, methodText]);
          }
        }
      } catch (error) {
        console.log('Error in processing:', error);
        // Optionally, you can decide whether to break or continue based on the error
      }
    }
  }

  // Write data to CSV
  const csvContent = data.map(row => row.join(',')).join('\n');
  fs.writeFileSync(path.join(__dirname, `${courseCode.replace(" ", "_")}.csv`), csvContent);

  await browser.close();
})();
