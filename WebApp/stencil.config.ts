import { Config } from '@stencil/core';
import { sass } from '@stencil/sass';

// https://stenciljs.com/docs/config

export const config: Config = {
  plugins: [
    sass()
  ],
  outputTargets: [{
    type: 'www',
    serviceWorker: {
      swSrc: 'src/sw.js',
      globPatterns: [
        '**/*.{js,css,json,html,ico,png,svg}'
      ]
    }
  }],
  globalScript: 'src/global/app.ts',
  globalStyle: 'src/global/app.css',
  taskQueue: 'async',
  copy: [{
    src: "**/*.i18n.*.json",
    dest: "i18n"
  }],
  devServer: {
    reloadStrategy: 'pageReload',
    port: 3333,
    openBrowser: false
  },
  enableCache: true,
  hashFileNames: true,
  hashedFileNameLength: 4,
  minifyJs: true,
  minifyCss: true,
  preamble: ""
};
