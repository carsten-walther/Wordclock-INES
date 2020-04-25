importScripts("https://storage.googleapis.com/workbox-cdn/releases/4.3.1/workbox-sw.js");

self.workbox.setConfig({
    debug: false
});

self.workbox.precaching.precacheAndRoute([
    {
        "url": "css/_form.scss"
    },
    {
        "url": "css/_general.scss"
    },
    {
        "url": "css/styles.css"
    },
    {
        "url": "css/styles.css.map"
    },
    {
        "url": "css/styles.css"
    },
    {
        "url": "img/apple-touch-icon.png"
    },
    {
        "url": "img/favicon.ico"
    },
    {
        "url": "img/icon-128x128.png"
    },
    {
        "url": "img/icon-192x192.png"
    },
    {
        "url": "img/icon-512x512.png"
    },
    {
        "url": "js/script.js"
    },
    {
        "url": "index.html"
    },
    {
        "url": "manifest.json"
    }
]);

self.addEventListener('message', ({data}) => {
    if (data === 'skipWaiting') {
        self.skipWaiting();
    }
});
