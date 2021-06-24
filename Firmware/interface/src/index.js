import React from 'react'
import ReactDOM from 'react-dom'

import Api from './utilities/Api'

import Captive from './Captive'
import App from './App'

import './assets/css/tailwind.css'
import './assets/css/override.css'

let isCaptivePortal = false

Api.getWifi().then((result) => {
    isCaptivePortal = result.payload.captivePortal
})

if (isCaptivePortal || window.location.host === '192.168.4.1' || window.location.hash === '#captive') {
    ReactDOM.render(<Captive/>, document.getElementById('wrapper'))
} else {
    ReactDOM.render(<App/>, document.getElementById('wrapper'))
}
