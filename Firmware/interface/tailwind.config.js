module.exports = {
    purge: {
        enabled: true,
        mode: 'all',
        preserveHtmlElements: false,
        content: [
            './src/**/*.{js,jsx,ts,tsx}',
            './public/index.html',
        ],
    },
    theme: {},
    variants: {
        extend: {
            opacity: ['disabled'],
        }
    },
    plugins: [],
}
