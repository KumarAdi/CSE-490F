let serial; // the Serial object
let serialOptions = { baudRate: 115200 };

let ballPos = {
    x: 50,
    y: 50
};

let ballRad = 20;

let joyStickPos = {
    x: 0,
    y: 0
}

function setup() {
    createCanvas(750, 420);

    // Setup Web Serial using serial.js
    serial = new Serial();
    serial.on(SerialEvents.CONNECTION_OPENED, onSerialConnectionOpened);
    serial.on(SerialEvents.CONNECTION_CLOSED, onSerialConnectionClosed);
    serial.on(SerialEvents.DATA_RECEIVED, onSerialDataReceived);
    serial.on(SerialEvents.ERROR_OCCURRED, onSerialErrorOccurred);

    // If we have previously approved ports, attempt to connect with them
    serial.autoConnectAndOpenPreviouslyApprovedPort(serialOptions);

    // Add in a lil <p> element to provide messages. This is optional
    pHtmlMsg = createP("Click anywhere on this page to open the serial connection dialog");

    background(50);
}

function draw() {
    background(50);
    ballPos.x += joyStickPos.x / 100;
    ballPos.y += joyStickPos.y / 100;

    ballRad = sqrt((joyStickPos.x * joyStickPos.x / 10000) + (joyStickPos.y * joyStickPos.y / 10000))

    ballPos.x = constrain(ballPos.x, ballRad, width - ballRad);
    ballPos.y = constrain(ballPos.y, ballRad, height - ballRad);

    circle(ballPos.x, ballPos.y, ballRad);
}

function onSerialErrorOccurred(eventSender, error) {
    console.log("onSerialErrorOccurred", error);
}

function onSerialConnectionOpened(eventSender) {
    console.log("onSerialConnectionOpened");
}

function onSerialConnectionClosed(eventSender) {
    console.log("onSerialConnectionClosed");
}

function onSerialDataReceived(eventSender, newData) {
    //console.log("onSerialDataReceived", newData);
    pHtmlMsg.html("onSerialDataReceived: " + newData);

    // JavaScript is not multithreaded, so we need not lock the queue
    // before pushing new elements
    const splitData = newData.split(',')
    joyStickPos = {
        x: parseInt(splitData[0]),
        y: parseInt(splitData[1])
    }
}

function mouseClicked() {
    if (!serial.isOpen()) {
        serial.connectAndOpen(null, serialOptions);
    }
}