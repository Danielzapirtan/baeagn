const CACHE_NAME = 'chess-analysis-v1';
const urlsToCache = [
  '/',
  '/index.html',
  '/manifest.json'
  // Add your specific files here
];

self.addEventListener('install', (event) => {
  event.waitUntil(
    caches.open(CACHE_NAME)
      .then((cache) => cache.addAll(urlsToCache))
  );
});

self.addEventListener('fetch', (event) => {
  // For your analysis artifacts, always try network first
  if (event.request.url.includes('analysis') || event.request.url.includes('artifact')) {
    event.respondWith(
      fetch(event.request)
        .then((response) => {
          // Cache the new analysis results
          const responseClone = response.clone();
          caches.open(CACHE_NAME)
            .then((cache) => cache.put(event.request, responseClone));
          return response;
        })
        .catch(() => caches.match(event.request)) // Fallback to cache if offline
    );
  } else {
    // For other resources, cache first
    event.respondWith(
      caches.match(event.request)
        .then((response) => response || fetch(event.request))
    );
  }
});