const BASE_URL = "http://localhost:3000/"

let model;
let myId;
let canvas;

let drawing = [];

let drawingDone = false;

function longPoll(url, options) {
    return fetch(url, options).then((resp) => {
        if (resp.status == 502) { // timeout
            console.log('timed out, retrying...');
            return longPoll(url, options);
        }

        return resp;
    });
}

function playRound() {
    console.log('fetching prompt...');
    longPoll(BASE_URL + "prompt", {
        mode: 'cors',
        headers: {
            'Access-Control-Allow-Origin': '*'
        }
    }).then((resp) => {
        console.log('received prompt!');
        resp.text().then((prompt) => {
            console.log("drawing a " + prompt);
            drawing = [];
            model = ml5.sketchRNN(prompt, modelReady);
        });
    });
}

function setup() {
    canvas = createCanvas(750, 420);
    background(200);

    console.log("registering...");
    fetch(BASE_URL + "players", {
        method: 'POST',
        mode: 'cors',
        headers: {
            'Access-Control-Allow-Origin': '*'
        },
        body: JSON.stringify({
            name: "bot",
            isBot: true
        })
    }).then((resp) => {
        console.log("registered!");
        resp.text().then((userId) => myId = userId);
    }).then(playRound);
}

// When the model is loaded
function modelReady() {
    // Reset the model's current stat
    model.reset();
    // Generate a new stroke
    model.generate().then(drawStroke);
}

function drawStroke(stroke) {
    drawing.push(stroke);

    if (stroke.pen !== "end") {
        model.generate().then(drawStroke);
    } else {
        drawingDone = true;
    }
}

function draw() {
    background(200);
    let penDown = true;
    let [x, y] = [width / 2, height / 2];
    for (let stroke of drawing) {
        if (penDown) {
            line(x, y, x + stroke.dx, y + stroke.dy);
        }
        x += stroke.dx;
        y += stroke.dy;
        penDown = stroke.pen === "down";
    }

    if (drawingDone) {
        console.log('submitting...');
        fetch(BASE_URL + "images/" + myId, {
            method: 'POST',
            mode: 'cors',
            headers: {
                'Access-Control-Allow-Origin': '*'
            },
            body: canvas.elt.toDataURL()
        }).then(() => {
            console.log("submitted!");
            setTimeout(playRound, 60 * 1000);
        });
        drawingDone = false;
    }
}