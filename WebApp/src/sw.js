importScripts("https://storage.googleapis.com/workbox-cdn/releases/5.1.3/workbox-sw.js");

self.workbox.setConfig({
  debug: false
});

self.workbox.precaching.precacheAndRoute([... self.__WB_MANIFEST]);

self.addEventListener('message', ({data}) => {
  if (data === 'skipWaiting') {
    self.skipWaiting();
  }
});
