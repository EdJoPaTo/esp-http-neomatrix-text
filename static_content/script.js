setInterval(updateStyle, 2500);
updateStyle();

async function updateStyle() {
    const response = await fetch('/json');
    const data = await response.json();
    console.log(data);
    const {hue, sat, bri, on, text} = data;

    updateBackground(hue, sat, on ? bri : 0);

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

function updateBackground(hue, sat, bri) {
    const [h, s, l] = hsv2hsl(hue, sat / 100, (bri / 255) ** 0.3);
    document.documentElement.style.setProperty('--background', `hsl(${hue}, ${s * 100}%, ${l * 100}%)`);
}

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

// https://stackoverflow.com/questions/3423214/convert-hsb-hsv-color-to-hsl
const hsv2hsl = (h,s,v,l=v-v*s/2, m=Math.min(l,1-l)) => [h,m?(v-l)/m:0,l];
