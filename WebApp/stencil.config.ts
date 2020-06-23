import { Config } from '@stencil/core';
import { sass } from '@stencil/sass';

// https://stenciljs.com/docs/config

export const config: Config = {
  buildEs5: false,
  bundles: [

  ],
  enableCache: true,
  globalScript: 'src/global/app.ts',
  globalStyle: 'src/global/app.css',
  hashFileNames: true,
  hashedFileNameLength: 4,
  namespace: "App",
  outputTargets: [{
    type: 'www',
    serviceWorker: null
  }],
  plugins: [
    sass()
  ],
  devServer: {
    port: 3333,
    reloadStrategy: 'pageReload',
    openBrowser: false
  },
  preamble: 'Made with ♥ by Carsten Walther',
  srcDir: 'src',
  taskQueue: 'congestionAsync',
  minifyJs: true,
  minifyCss: true,
  extras: {
    cssVarsShim: false,
    dynamicImportShim: false,
    safari10: false,
    scriptDataOpts: false,
    shadowDomShim: false
  },
};
