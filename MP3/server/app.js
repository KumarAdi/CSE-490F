// taken from https://github.com/ml5js/ml5-library/blob/main/src/SketchRNN/models.js
//because ml5 can't be imported without a browser
const models = [
    'alarm_clock',
    'ambulance',
    'angel',
    'ant',
    'antyoga',
    'backpack',
    'barn',
    'basket',
    'bear',
    'bee',
    'beeflower',
    'bicycle',
    'bird',
    'book',
    'brain',
    'bridge',
    'bulldozer',
    'bus',
    'butterfly',
    'cactus',
    'calendar',
    'castle',
    'cat',
    'catbus',
    'catpig',
    'chair',
    'couch',
    'crab',
    'crabchair',
    'crabrabbitfacepig',
    'cruise_ship',
    'diving_board',
    'dog',
    'dogbunny',
    'dolphin',
    'duck',
    'elephant',
    'elephantpig',
    'eye',
    'face',
    'fan',
    'fire_hydrant',
    'firetruck',
    'flamingo',
    'flower',
    'floweryoga',
    'frog',
    'frogsofa',
    'garden',
    'hand',
    'hedgeberry',
    'hedgehog',
    'helicopter',
    'kangaroo',
    'key',
    'lantern',
    'lighthouse',
    'lion',
    'lionsheep',
    'lobster',
    'map',
    'mermaid',
    'monapassport',
    'monkey',
    'mosquito',
    'octopus',
    'owl',
    'paintbrush',
    'palm_tree',
    'parrot',
    'passport',
    'peas',
    'penguin',
    'pig',
    'pigsheep',
    'pineapple',
    'pool',
    'postcard',
    'power_outlet',
    'rabbit',
    'rabbitturtle',
    'radio',
    'radioface',
    'rain',
    'rhinoceros',
    'rifle',
    'roller_coaster',
    'sandwich',
    'scorpion',
    'sea_turtle',
    'sheep',
    'skull',
    'snail',
    'snowflake',
    'speedboat',
    'spider',
    'squirrel',
    'steak',
    'stove',
    'strawberry',
    'swan',
    'swing_set',
    'the_mona_lisa',
    'tiger',
    'toothbrush',
    'toothpaste',
    'tractor',
    'trombone',
    'truck',
    'whale',
    'windmill',
    'yoga',
    'yogabicycle',
    'everything',
];

const DRAW_TIME_LIMIT = 60 * 1000;
const VOTE_TIME_LIMIT = DRAW_TIME_LIMIT;

const express = require('express')
const cors = require('cors')
const {
    v1: uuidv1,
    v4: uuidv4
} = require('uuid');
const e = require('express');
const app = express()
const port = process.env.PORT || 80;

const GameStateEnum = Object.freeze({ WAITING: "waiting", DRAWING: "drawing", VOTING: "voting" });

let gameState = GameStateEnum.WAITING;
let users = {};
let prompt;
let waitingForPrompt = {};
let waitingForImages = {};
let nextStateTimer;

app.use(cors());
// JSON formatting might be annoying on the microcontroller, so I'll leave it as text and parse in the API endpoint if need be
app.use(express.text());

app.use(express.static('clients'));

app.post('/reset', (req, res) => {
    console.log('Starting new game...');
    if (nextStateTimer) {
        clearTimeout(nextStateTimer);
    }
    gameState = GameStateEnum.WAITING;
    users = {};
});

app.post('/start', (req, res) => {
    if (gameState === GameStateEnum.WAITING) {
        gameState = GameStateEnum.DRAWING;
        prompt = models[Math.floor(Math.random() * models.length)];
        Object.values(waitingForPrompt).forEach(waiting => {
            waiting.send(prompt);
            waiting.end();
        });
        waitingForPrompt = {};
        nextStateTimer = setTimeout(() => {
            gameState = GameStateEnum.VOTING;
            const images = getImages();
            Object.values(waitingForImages).forEach(waiting => {
                waiting.send(images);
                waiting.end();
            });
            waitingForImages = {};
            nextStateTimer = setTimeout(() => {
                gameState = GameStateEnum.WAITING;
                nextStateTimer = undefined;
                console.log('voting end');
            }, VOTE_TIME_LIMIT);
            console.log('voting start');
        }, DRAW_TIME_LIMIT);
        console.log('round start, prompt is ' + prompt);
        res.send(prompt);
    } else {
        res.sendStatus(403);
    }
});

app.post('/players', (req, res) => {
    const userData = JSON.parse(req.body);
    userData.id = uuidv4();
    userData.score = 0;
    users[userData.id] = userData;
    console.log(userData.name + ' joined the game')
    res.send(userData.id);
});

app.get('/players/:userId', (req, res) => {
    const userId = req.params["userId"];
    if (users[userId]) {
        res.send(users[userId]);
    } else {
        res.sendStatus(404);
    }
});

app.get('/prompt', (req, res) => {
    if (gameState === GameStateEnum.DRAWING) {
        res.send(prompt);
    } else {
        let id = uuidv4();
        waitingForPrompt[id] = res;
        res.on("close", () => {
            delete waitingForPrompt[id];
        })
    }
});

app.get('/images', (req, res) => {
    if (gameState === GameStateEnum.VOTING) {
        res.send(getImages());
    } else {
        let id = uuidv4();
        waitingForImages[id] = res;
        res.on("close", () => {
            delete waitingForPrompt[id];
        })
    }
})

app.post('/images/:userId', (req, res) => {
    const userId = req.params["userId"];
    if (gameState === GameStateEnum.DRAWING) {
        if (users[userId]) {
            console.log("received image from " + users[userId].name)
            users[userId].image = req.body;
            res.sendStatus(201);
        } else {
            console.log(userId + " is not in " + Object.keys(users));
            res.sendStatus(404);
        }
    } else {
        res.sendStatus(403);
    }
});

app.post('/votes/:userId', (req, res) => {
    const userId = req.params["userId"];
    if (gameState === GameStateEnum.VOTING) {
        if (users[userId]) {
            if (users[req.body].isBot) {
                users[userId].score += 1;
            }
            console.log(users[userId].name + " voted for " + users[req.body].name);
            res.send({
                voted_for: users[req.body].name,
                bot: Object.values(users).filter(user => user.isBot)[0]
            });
        } else {
            res.sendStatus(404);
        }
    } else {
        res.sendStatus(403);
    }
});

app.listen(port, () => {
    console.log(`App running on port ${port}`)
})

function getImages() {
    let images = Object.values(users).map(user => ({ id: user.id, image: user.image })).filter(img => img);

    // Dursrtenfeld Shuffle
    for (let i = images.length - 1; i > 0; i--) {
        const rand = Math.floor(Math.random() * (i + 1));
        [images[i], images[rand]] = [images[rand], images[i]];
    }
    return images;
}
