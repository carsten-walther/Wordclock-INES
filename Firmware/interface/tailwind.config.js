module.exports = {
    purge: {
        enabled: true,
        mode: 'all',
        preserveHtmlElements: false,
        content: [
            //'./src/**/*.{css,sass,scss}',
            './src/**/*.{js,jsx,ts,tsx}',
            './public/index.html',
        ],
    },
    theme: {},
    variants: {},
    plugins: [],
}
