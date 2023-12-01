const puppeteer = require('puppeteer');
const fs = require('fs');
const path = require('path');

(async () => {
    const browser = await puppeteer.launch({});
    const page = await browser.newPage();
    const courseCode = process.argv[2]; // Gets the third command line argument
    if (!courseCode) {
        console.log("Please provide a course code.");
        return;
    }
    const [coursePrefix, courseNumber] = courseCode.split(' ');

    await page.goto(`https://colleague-ss.uoguelph.ca/Student/Courses/Search?keyword=${courseCode.replace(" ", "+")}`, { waitUntil: 'networkidle2' });

    const elements = await page.$$('[id]');
    for (const element of elements) {
        const id = await element.evaluate(el => el.getAttribute('id'));
        if (id && id.includes(`collapsible-view-available-sections-for-${coursePrefix}`) && id.includes(`${courseNumber}-groupHeading`)) {
            await element.click();
            break;
        }
    }

    await page.waitForXPath("//*[contains(@id,'section-') and contains(@id,'-meeting-')]");

    const sectionElements = await page.$$("[id^='section-'][id*='-meeting-']");
    const uniqueIds = new Set();

    for (const element of sectionElements) {
        const id = await element.evaluate(node => node.id);
        const match = id.match(/section-(\d+)-meeting/);
        if (match) {
            uniqueIds.add(match[1]);
        }
    }

    const uniqueRows = new Set();
    const data = [];

    for (const sectionId of uniqueIds) {
        let sectionText = await page.evaluate(element => element.textContent, await page.$(`#section-${sectionId}`));
        sectionText = sectionText.replace(new RegExp(`\\b${courseNumber}\\b`, 'g'), '');
        sectionText = sectionText.replace(new RegExp(`\\b${coursePrefix}\\b`, 'g'), '');
        sectionText = sectionText.replace(/\*/g, '');

        for (let i = 0; i < 4; i++) {
            try {
                const startTimeSelector = `#section-${sectionId}-meeting-times-start-${i}`;
                const endTimeSelector = `#section-${sectionId}-meeting-times-end-${i}`;
                const daysSelector = `#section-${sectionId}-meeting-days-${i}`;
                const methodSelector = `#section-${sectionId}-meeting-instructional-method-${i}`;

                if (await page.$(startTimeSelector)) {
                    const startTimeText = await page.evaluate(element => element.textContent, await page.$(startTimeSelector));
                    const endTimeText = await page.evaluate(element => element.textContent, await page.$(endTimeSelector));
                    let daysText = await page.evaluate(element => element.textContent, await page.$(daysSelector));
                    const methodText = await page.evaluate(element => element.textContent, await page.$(methodSelector));

                    if (methodText.trim() === "EXAM") {
                        continue;
                    }
                    daysText = daysText.replace(/T(?!h)/g, 'Tu')

                    const key = `${sectionText}-${startTimeText}-${endTimeText}-${daysText}-${methodText}`;

                    if (!uniqueRows.has(key)) {
                        uniqueRows.add(key);
                        data.push([sectionText, startTimeText, endTimeText, daysText, methodText]);
                    }
                }
            } catch (error) {
                console.log('Error in processing:', error);
            }
        }
    }

    // Write data to CSV
    const csvContent = data.map(row => row.join(',')).join('\n');
    fs.writeFileSync(path.join(__dirname, `${courseCode.replace(" ", "_")}.csv`), csvContent);

    await browser.close();
})();
