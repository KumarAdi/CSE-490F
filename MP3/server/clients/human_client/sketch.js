const BASE_URL = "/"

let canvas;
let myId;

function longPoll(url, options) {
    return fetch(url, options).then((resp) => {
        if (resp.status == 502) { // timeout
            console.log('timed out, retrying...');
            return longPoll(url, options);
        }
        return resp;
    });
}

function hideAll() {
    document.getElementById('signup').style.display = 'none';
    document.getElementById('waiting_begin').style.display = 'none';
    document.getElementById('waiting_end').style.display = 'none';
    document.getElementById('voting').style.display = 'none';
    document.getElementById('drawing').style.display = 'none';
    document.getElementById('timer').style.display = 'none';
    canvas.elt.style.display = 'none';
}

function submitImage() {
    console.log('submitting image...')
    fetch(BASE_URL + "images/" + myId, {
        method: 'POST',
        mode: 'cors',
        headers: {
            'Access-Control-Allow-Origin': '*'
        },
        body: canvas.elt.toDataURL()
    }).then(() => {
        console.log("submitted!");
        hideAll();
        document.getElementById('waiting_end').style.display = 'block';
    })
}

function submitVote(imageID) {
    console.log('submitting vote for ' + imageID)
    fetch(BASE_URL + "votes/" + myId, {
        method: 'POST',
        mode: 'cors',
        headers: {
            'Access-Control-Allow-Origin': '*'
        },
        body: imageID
    }).then((res) => {
        console.log("submitted!");
        hideAll();
        res.json().then((votingInfo) => {
            document.getElementById('voting_results').innerHTML = "";

            const textElem = document.createElement('p');
            textElem.innerHTML = "You voted for " + votingInfo.voted_for + ". Here is the bot's image:"

            const imgElem = document.createElement('img');
            imgElem.src = votingInfo.bot.image;
            imgElem.width = 400;

            document.getElementById('voting_results').appendChild(textElem)
            document.getElementById('voting_results').appendChild(imgElem)
            document.getElementById('waiting_begin').style.display = 'block';
        }).then(() => {
            fetch(BASE_URL + 'players/' + myId).then(
                res => res.json().then((user) =>
                    document.getElementById("score").innerText = "You have " + user.score + " points"));
        });
    })
}

function startVoting() {
    hideAll();
    document.getElementById('waiting_end').style.display = 'block';
    longPoll(BASE_URL + 'images', {
        mode: 'cors',
        headers: {
            'Access-Control-Allow-Origin': '*'
        }
    }).then(images => {
        console.log(images);
        hideAll();
        let votingDiv = document.getElementById('voting');
        // https://stackoverflow.com/a/22966637
        var cNode = votingDiv.cloneNode(false);
        votingDiv.parentNode.replaceChild(cNode, votingDiv);
        votingDiv = cNode;
        images.json().then(imageArr => imageArr.filter(img => img.id != myId).forEach((img) => {
            const imgElem = document.createElement('img');
            imgElem.src = img.image;
            imgElem.width = 200;
            imgElem.onclick = () => { submitVote(img.id) };
            votingDiv.appendChild(imgElem)
            imgElem.style.margin = '20px 20px 20px 20px'
        }));
        votingDiv.style.display = 'block';
        document.getElementById('timer-time').innerHTML = '58';
        document.getElementById('timer').style.display = 'block';
        setTimeout(startDrawing, 58 * 1000);
    })
}

function startDrawing() {
    hideAll();
    document.getElementById('waiting_begin').style.display = 'block';
    longPoll(BASE_URL + 'prompt', {
        mode: 'cors',
        headers: {
            'Access-Control-Allow-Origin': '*'
        }
    }).then(() => {
        hideAll();
        background(200);
        canvas.elt.style.display = 'block';
        document.getElementById('drawing').style.display = 'block';
        document.getElementById('timer-time').innerHTML = '58';
        document.getElementById('timer').style.display = 'block';
        setTimeout(startVoting, 58 * 1000);
    })
}

document.onload = () => document.getElementById('username_box').onkeypress = () => {
    if (!e) e = window.event;
    var keyCode = e.code || e.key;
    if (keyCode == 'Enter') {
        registerUser(document.getElementById('username_box').value);
        return false;
    }
}

function registerUser(username) {
    hideAll();
    fetch(BASE_URL + "players", {
        method: 'POST',
        mode: 'cors',
        headers: {
            'Access-Control-Allow-Origin': '*'
        },
        body: JSON.stringify({
            name: username,
            isBot: false
        })
    }).then((resp) => {
        resp.text().then((userId) => myId = userId);
    }).then(startDrawing);
}

function setup() {
    canvas = createCanvas(750, 420);
    hideAll();
    document.getElementById("signup").style.display = 'block';
    background(200);
    setInterval(() => {
        document.getElementById('timer-time').innerHTML = parseInt(document.getElementById('timer-time').innerHTML) - 1;
    }, 1000);
}

function draw() {
}

function mouseDragged() {
    strokeWeight(1);
    line(mouseX, mouseY, pmouseX, pmouseY)
}