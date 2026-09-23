const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { chromium } = require('playwright');

const projectRoot = path.resolve(__dirname, '..');
const dashboardRoot = path.join(projectRoot, 'data');
const dashboardUrl = 'http://192.168.1.200/';
const systemBrowserCandidates = [
    '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome',
    '/Applications/Chromium.app/Contents/MacOS/Chromium',
    '/Applications/Microsoft Edge.app/Contents/MacOS/Microsoft Edge'
];

const contentTypes = {
    '.css': 'text/css',
    '.html': 'text/html',
    '.js': 'text/javascript',
    '.png': 'image/png',
    '.svg': 'image/svg+xml'
};

async function createDashboardPage(browser, options = {}) {
    const page = await browser.newPage();
    const consoleErrors = [];
    page.on('console', message => {
        if (message.type() === 'error') {
            consoleErrors.push(message.text());
        }
    });

    await page.addInitScript(({ fetchFails }) => {
        window.__dashboardTest = {
            fetchFails,
            sockets: [],
            timeouts: []
        };

        window.fetch = () => window.__dashboardTest.fetchFails
            ? Promise.reject(new Error('controller offline'))
            : Promise.resolve({ ok: true });

        window.setTimeout = (callback, delay, ...args) => {
            window.__dashboardTest.timeouts.push({ callback, delay, args });
            return window.__dashboardTest.timeouts.length;
        };

        class MockWebSocket {
            constructor(url) {
                this.url = url;
                this.sent = [];
                window.__dashboardTest.sockets.push(this);
            }

            send(message) {
                this.sent.push(message);
            }

            open() {
                this.onopen?.({ type: 'open' });
            }

            close() {
                this.onclose?.({ type: 'close' });
            }

            receive(data) {
                this.onmessage?.({ data });
            }
        }

        window.WebSocket = MockWebSocket;
        window.__runNextDashboardTimeout = () => {
            const timeout = window.__dashboardTest.timeouts.shift();
            timeout?.callback(...timeout.args);
            return timeout?.delay;
        };
    }, { fetchFails: options.fetchFails ?? false });

    await page.route('**/*', async route => {
        const requestUrl = new URL(route.request().url());
        const relativePath = requestUrl.pathname === '/'
            ? 'index.html'
            : requestUrl.pathname.slice(1);
        const filePath = path.join(dashboardRoot, relativePath);

        if (!filePath.startsWith(`${dashboardRoot}${path.sep}`) || !fs.existsSync(filePath)) {
            await route.fulfill({ status: 404, body: 'Not found' });
            return;
        }

        await route.fulfill({
            status: 200,
            contentType: contentTypes[path.extname(filePath)] ?? 'application/octet-stream',
            body: fs.readFileSync(filePath)
        });
    });

    await page.goto(dashboardUrl);
    await page.waitForFunction(() => window.__dashboardTest.sockets.length === 1);
    return { page, consoleErrors };
}

async function testInitialConnectionAndActiveTab(browser) {
    const { page, consoleErrors } = await createDashboardPage(browser);

    const state = await page.evaluate(() => ({
        socketUrl: window.__dashboardTest.sockets[0].url,
        activeTabs: [...document.querySelectorAll('.tab.active')].map(tab => tab.dataset.ip),
        statusDisplay: document.getElementById('connection-status').style.display
    }));

    assert.equal(state.socketUrl, 'ws://192.168.1.200/ws');
    assert.deepEqual(state.activeTabs, ['192.168.1.200']);
    assert.equal(state.statusDisplay, 'none');
    assert.deepEqual(consoleErrors, []);
    await page.close();
}

async function testOpenRequestsAndRendersAllValues(browser) {
    const { page, consoleErrors } = await createDashboardPage(browser);
    const payload = {
        sliderValue1: '21.25',
        sliderValue2: '-4.50',
        sliderValue3: '38.00',
        sliderValue4: '125.75',
        sliderValue5: '12.60',
        sliderValue6: '87.00',
        sliderValue1AgeMinutes: 0,
        sliderValue2AgeMinutes: 34,
        sliderValue3AgeMinutes: 94,
        sliderValue4AgeMinutes: 1440,
        sliderValue5AgeMinutes: 2880,
        sliderValue6AgeMinutes: null
    };

    const result = await page.evaluate(values => {
        const socket = window.__dashboardTest.sockets[0];
        socket.open();
        socket.receive(JSON.stringify(values));

        return {
            sent: socket.sent,
            statusDisplay: document.getElementById('connection-status').style.display,
            rendered: Object.fromEntries(
                Object.keys(values)
                    .filter(id => !id.endsWith('AgeMinutes'))
                    .map(id => [id, document.getElementById(id)?.textContent])
            ),
            renderedAges: Object.fromEntries(
                Object.keys(values)
                    .filter(id => id.endsWith('AgeMinutes'))
                    .map(id => {
                        const ageId = id.replace('AgeMinutes', 'Age');
                        return [ageId, document.getElementById(ageId)?.textContent];
                    })
            ),
            staleValues: Object.fromEntries(
                Object.keys(values)
                    .filter(id => !id.endsWith('AgeMinutes'))
                    .map(id => [
                        id,
                        document.getElementById(id)?.closest('.sensor-value')
                            ?.classList.contains('is-stale')
                    ])
            )
        };
    }, payload);

    assert.deepEqual(result.sent, ['getValues']);
    assert.equal(result.statusDisplay, 'none');
    assert.deepEqual(result.rendered, {
        sliderValue1: '21.25',
        sliderValue2: '-4.50',
        sliderValue3: '38.00',
        sliderValue4: '125.75',
        sliderValue5: '12.60',
        sliderValue6: '87.00'
    });
    assert.deepEqual(result.renderedAges, {
        sliderValue1Age: 'оновлено: щойно',
        sliderValue2Age: 'оновлено: 34 хв тому',
        sliderValue3Age: 'оновлено: 1 год 34 хв тому',
        sliderValue4Age: 'оновлено: 1 день тому',
        sliderValue5Age: 'оновлено: 2 дні тому',
        sliderValue6Age: 'ще не оновлено'
    });
    assert.deepEqual(result.staleValues, {
        sliderValue1: false,
        sliderValue2: false,
        sliderValue3: true,
        sliderValue4: true,
        sliderValue5: true,
        sliderValue6: false
    });
    assert.deepEqual(consoleErrors, []);
    await page.close();
}

async function testFormatsAgeBoundariesAndRefreshesLocally(browser) {
    const { page, consoleErrors } = await createDashboardPage(browser);

    const result = await page.evaluate(() => {
        let now = 1000000;
        Date.now = () => now;

        const socket = window.__dashboardTest.sockets[0];
        socket.receive(JSON.stringify({ sliderValue1AgeMinutes: 59 }));
        const beforeRefresh = document.getElementById('sliderValue1Age').textContent;
        const staleBeforeRefresh = document.querySelector('#sliderValue1')
            .closest('.sensor-value').classList.contains('is-stale');
        now += 60000;
        refreshUpdateAges();
        const afterRefresh = document.getElementById('sliderValue1Age').textContent;
        const staleAfterRefresh = document.querySelector('#sliderValue1')
            .closest('.sensor-value').classList.contains('is-stale');
        socket.receive(JSON.stringify({ sliderValue1AgeMinutes: 0 }));

        return {
            beforeRefresh,
            staleBeforeRefresh,
            afterRefresh,
            staleAfterRefresh,
            staleAfterFreshUpdate: document.querySelector('#sliderValue1')
                .closest('.sensor-value').classList.contains('is-stale'),
            exactHour: formatUpdateAge(60),
            underDay: formatUpdateAge(1439),
            roundsDownToDay: formatUpdateAge(2159),
            roundsUpToDays: formatUpdateAge(2160),
            elevenDays: formatUpdateAge(15840),
            twentyOneDays: formatUpdateAge(30240)
        };
    });

    assert.deepEqual(result, {
        beforeRefresh: 'оновлено: 59 хв тому',
        staleBeforeRefresh: false,
        afterRefresh: 'оновлено: 1 год тому',
        staleAfterRefresh: true,
        staleAfterFreshUpdate: false,
        exactHour: 'оновлено: 1 год тому',
        underDay: 'оновлено: 23 год 59 хв тому',
        roundsDownToDay: 'оновлено: 1 день тому',
        roundsUpToDays: 'оновлено: 2 дні тому',
        elevenDays: 'оновлено: 11 днів тому',
        twentyOneDays: 'оновлено: 21 день тому'
    });
    assert.deepEqual(consoleErrors, []);
    await page.close();
}

async function testUnknownAndInvalidPayloadsAreSafe(browser) {
    const { page, consoleErrors } = await createDashboardPage(browser);

    const result = await page.evaluate(() => {
        const socket = window.__dashboardTest.sockets[0];
        socket.receive(JSON.stringify({ sliderValue1: '19.50', missingElement: 'ignored' }));
        socket.receive('{invalid json');

        return {
            knownValue: document.getElementById('sliderValue1').textContent,
            missingElement: document.getElementById('missingElement')
        };
    });

    assert.equal(result.knownValue, '19.50');
    assert.equal(result.missingElement, null);
    assert.equal(consoleErrors.length, 1);
    assert.match(consoleErrors[0], /Invalid WebSocket payload/);
    await page.close();
}

async function testCloseShowsWarningAndReconnects(browser) {
    const { page, consoleErrors } = await createDashboardPage(browser);

    const result = await page.evaluate(() => {
        window.__dashboardTest.sockets[0].open();
        window.__dashboardTest.sockets[0].close();
        const scheduledDelay = window.__runNextDashboardTimeout();

        return {
            scheduledDelay,
            socketCount: window.__dashboardTest.sockets.length,
            reconnectUrl: window.__dashboardTest.sockets[1]?.url,
            statusDisplay: document.getElementById('connection-status').style.display
        };
    });

    assert.equal(result.scheduledDelay, 2000);
    assert.equal(result.socketCount, 2);
    assert.equal(result.reconnectUrl, 'ws://192.168.1.200/ws');
    assert.equal(result.statusDisplay, 'block');
    assert.deepEqual(consoleErrors, []);
    await page.close();
}

async function testFailedHealthCheckShowsOfflineMessage(browser) {
    const { page, consoleErrors } = await createDashboardPage(browser, { fetchFails: true });
    await page.waitForFunction(() => (
        document.getElementById('connection-status').textContent.includes('Контролер недоступний')
    ));

    const status = await page.locator('#connection-status').evaluate(element => ({
        display: element.style.display,
        text: element.textContent
    }));

    assert.equal(status.display, 'block');
    assert.match(status.text, /Контролер недоступний/);
    assert.deepEqual(consoleErrors, []);
    await page.close();
}

const tests = [
    ['initial connection and active tab', testInitialConnectionAndActiveTab],
    ['open requests and renders all values', testOpenRequestsAndRendersAllValues],
    ['formats age boundaries and refreshes locally', testFormatsAgeBoundariesAndRefreshesLocally],
    ['unknown and invalid payloads are safe', testUnknownAndInvalidPayloadsAreSafe],
    ['close shows warning and reconnects', testCloseShowsWarningAndReconnects],
    ['failed health check shows offline message', testFailedHealthCheckShowsOfflineMessage]
];

function browserLaunchOptions() {
    const executablePath = process.env.BROWSER_EXECUTABLE
        || systemBrowserCandidates.find(candidate => fs.existsSync(candidate));

    return executablePath ? { headless: true, executablePath } : { headless: true };
}

(async () => {
    const browser = await chromium.launch(browserLaunchOptions());
    let failures = 0;

    try {
        for (const [name, test] of tests) {
            try {
                await test(browser);
                console.log(`PASS ${name}`);
            } catch (error) {
                failures++;
                console.error(`FAIL ${name}`);
                console.error(error);
            }
        }
    } finally {
        await browser.close();
    }

    console.log(`${tests.length - failures}/${tests.length} browser tests passed`);
    process.exitCode = failures === 0 ? 0 : 1;
})().catch(error => {
    console.error(error);
    process.exitCode = 1;
});
