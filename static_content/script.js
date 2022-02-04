console.log('i was here');

async function updateStyle() {
    const data = await (await fetch('/json')).json();
    console.log(data);
    const {hue, sat, bri, on, text} = data;

    document.documentElement.style.setProperty('--hue', hue);
    document.documentElement.style.setProperty('--sat', `${sat}%`);

    document.getElementById('on').checked = on;
    updateElementValueById('hue', hue);
    updateElementValueById('sat', sat);
    updateElementValueById('bri', bri);
    updateElementValueById('text', text);

    document.getElementById("huetext").innerText = hue;
    document.getElementById("sattext").innerText = sat;
    document.getElementById("britext").innerText = bri;
    document.getElementById("texttext").innerText = text;
}

setInterval(updateStyle, 2500);
updateStyle();

function updateElementValueById(id, newValue) {
    const element = document.getElementById(id);
    if (element !== document.activeElement) {
        element.value = newValue;
    }
}

async function doPost(method, value) {
    await fetch(method, {
        method: 'POST',
        body: String(value),
    });
}

async function set(method, value) {
    await doPost(method, value);
    await updateStyle();
}
