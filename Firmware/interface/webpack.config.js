const path = require('path')
const postcssImport = require('postcss-import')
const tailwindcss = require('tailwindcss')
const autoprefixer = require('autoprefixer')
const HtmlWebpackPlugin = require('html-webpack-plugin')
const { CleanWebpackPlugin } = require('clean-webpack-plugin')
const TerserPlugin = require('terser-webpack-plugin')
const CssMinimizerPlugin = require('css-minimizer-webpack-plugin')
const CompressionPlugin = require('compression-webpack-plugin')

module.exports = {
    target: 'web',
    devServer: {
        historyApiFallback: true,
        open: false,
        compress: true,
        hot: true,
        port: 3000,
    },
    entry: {
        index: path.join(__dirname, 'src/index.js'),
    },
    module: {
        rules: [
            {
                test: /\.(css|sass|scss)$/,
                use: [
                    'style-loader',
                    {
                        loader: 'css-loader',
                        options: {
                            importLoaders: 1,
                        },
                    },
                    {
                        loader: 'postcss-loader',
                        options: {
                            postcssOptions: {
                                ident: 'postcss',
                                plugins: [
                                    postcssImport,
                                    tailwindcss,
                                    autoprefixer
                                ],
                            },
                        },
                    },
                    'sass-loader',
                ],
            },
            {
                test: /\.(js|jsx)$/,
                exclude: /node_modules/,
                use: [
                    {
                        loader: 'babel-loader',
                        options: {
                            presets: [
                                '@babel/preset-env',
                                '@babel/preset-react',
                            ],
                            plugins: [
                                [
                                    '@babel/plugin-transform-runtime',
                                    {
                                        regenerator: true,
                                    },
                                ],
                            ],
                        },
                    },
                ],
            },
            {
                test: /\.html$/,
                use: [
                    {
                        loader: 'html-loader',
                        options: {
                            minimize: true,
                        },
                    },
                ],
            },
            {
                test: /\.(png|jpe?g|gif|svg)$/i,
                use: [
                    {
                        loader: 'file-loader',
                        options: {
                            name: '[name].[ext]',
                            outputPath: 'assets/',
                        },
                    },
                ],
            },
        ],
    },
    plugins: [
        new CleanWebpackPlugin({
            cleanStaleWebpackAssets: true,
        }),
        new HtmlWebpackPlugin({
            template: path.resolve(__dirname, 'public/index.html'),
            filename: 'index.html',
            inlineSource: '.(js|css)$',
        }),
        new CompressionPlugin({
            algorithm: 'gzip',
            filename: '[path][base].gz',
            compressionOptions: {
                level: 9
            },
            deleteOriginalAssets: true
        })
    ],
    optimization: {
        minimize: true,
        runtimeChunk: 'single',
        minimizer: [
            new TerserPlugin({
                extractComments: false
            }),
            new CssMinimizerPlugin(),
        ],
    },
    performance: {
        hints: false,
        maxEntrypointSize: 512000,
        maxAssetSize: 512000,
    },
    output: {
        filename: '[name].js',
        path: path.resolve(__dirname, '../data'),
    },
}
