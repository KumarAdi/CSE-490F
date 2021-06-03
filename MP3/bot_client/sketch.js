let model;

const drawing = [];

function setup() {
    createCanvas(750, 420);
    background(200);

    fetch("http://localhost:3000/prompt", {
        mode: 'cors',
        headers: {
            'Access-Control-Allow-Origin': '*'
        }
    }).then((resp) => {
        resp.text().then((prompt) => {
            console.log("drawing a " + prompt)
            model = ml5.sketchRNN(prompt, modelReady);
        });
    });
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
}