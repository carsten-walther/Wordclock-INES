import React from 'react'
import { Link } from 'react-scroll'
import { BrowserRouter as Router, Route, Routes } from 'react-router-dom'

import Captive from './pages/Captive'
import ColorAndBrightness from './pages/ColorAndBrightness'
import ModeAndLanguage from './pages/ModeAndLanguage'
import TimeSettings from './pages/TimeSettings'
import OnOffTime from './pages/OnOffTime'
import Network from './pages/Network'
import Accessibility from './pages/Accessibility'
import Security from './pages/Security'
import System from './pages/System'
import Licences from './pages/Licences'

import Api from './utilities/Api'
import Color from './utilities/Color'
import Utility from './utilities/Utility'

import Sidebar from './components/Sidebar'
import Loader from './components/Loader'

import ChevronUp from './components/icons/ChevronUp'

export default class App extends React.Component {

    constructor (props) {
        super(props)
        this.state = {
            showScrollToTop: false,
            showExpertMode: false,
            isLoading: true,

            file: null,

            data: {
                foreground: {
                    r: 0,
                    g: 0,
                    b: 0,
                },
                background: {
                    r: 0,
                    g: 0,
                    b: 0,
                },
                brightness: 0,
                mode: 0,
                language: 0,
                ntpServer: '',
                ntpSyncInterval: 0,
                timezone: 0,
                daylightSavingTime: false,
                sleep: {
                    h: 0,
                    m: 0,
                },
                wakeup: {
                    h: 0,
                    m: 0,
                },
                hostname: '',
                useAuth: false,
                authUsername: '',
                authPassword: '',
            },

            wifi: {
                ssid: '',
                ip: '',
                sub: '',
                gw: '',
                dns: '',
                captivePortal: false,
            },

            info: {
                version: '',
                mac: '',
            },

            networks: [],
        }
    }

    async componentDidMount () {
        document.addEventListener('scroll', this.onScroll.bind(this))

        await this.wifiGet()
        await this.configGet()
        await this.systemInfo()

        if (this.state.wifi.captivePortal) {
            await this.wifiScan()
        }
    }

    // api/wifi
    //
    //

    async wifiGet () {
        await Api.wifiGet().then((result) => {
            this.setState({
                isLoading: false,
                wifi: result.payload,
            })
        })
    }

    async wifiSet () {
        if (this.state.static) {
            await Api.wifiSetStatic(this.state.data).then((result) => {
                this.setState({
                    isLoading: false,
                })
            })
        } else {
            await Api.wifiSet(this.state.data).then((result) => {
                this.setState({
                    isLoading: false,
                })
            })
        }
    }

    async wifiScan () {
        await Api.wifiScan().then((result) => {
            this.setState({
                isLoading: false,
                networks: result.payload.networks,
            })
        })
    }

    async wifiForget () {
        await Api.wifiForget().then((result) => {
            this.setState({
                isLoading: true,
            })
        })
    }

    // api/config
    //
    //

    async configGet () {
        await Api.configGet().then(result => {
            this.setState({
                isLoading: false,
                data: result.payload,
            })
        })
    }

    async configSet () {
        let returnVal = []

        Object.keys(this.state.data).map((key) => {
            if (key === 'foreground') {
                returnVal['foregroundRed'] = this.state.data[key].r
                returnVal['foregroundGreen'] = this.state.data[key].g
                returnVal['foregroundBlue'] = this.state.data[key].b
            } else if (key === 'background') {
                returnVal['backgroundRed'] = this.state.data[key].r
                returnVal['backgroundGreen'] = this.state.data[key].g
                returnVal['backgroundBlue'] = this.state.data[key].b
            } else if (key === 'sleep') {
                returnVal['sleepHour'] = this.state.data[key].h
                returnVal['sleepMinute'] = this.state.data[key].m
            } else if (key === 'wakeup') {
                returnVal['wakeupHour'] = this.state.data[key].h
                returnVal['wakeupMinute'] = this.state.data[key].m
            } else {
                returnVal[key] = this.state.data[key]
            }
            return null
        })

        await Api.configSet(returnVal).then(async (result) => {
            this.setState({
                isLoading: false,
            })
            await this.configGet()
        })
    }

    // api/system
    //
    //

    async systemInfo () {
        await Api.systemInfo().then((result) => {
            this.setState({
                isLoading: false,
                info: { ...result.payload },
            })
        })
    }

    async systemRestart () {
        await Api.systemRestart().then((result) => {
            this.setState({
                isLoading: true,
            })
        })
    }

    // ...

    async handleChange (event) {
        let fieldName = event.target.name ? event.target.name : event.target.id
        let fieldValue = null
        switch (event.target.type) {
            default:
                fieldValue = event.target.value
                break
            case 'file':
                fieldValue = event.target.files[0]
                break
            case 'checkbox':
                fieldValue = !!event.target.checked
                break
            case 'radio':
                fieldValue = event.target.value
                break
        }

        if (fieldName === 'foreground' || fieldName === 'background') {
            fieldValue = Color.hex2rgb(fieldValue)
            this.setState({
                data: { ...this.state.data, [fieldName]: fieldValue },
            })
        } else if (fieldName === 'sleep' || fieldName === 'wakeup') {
            fieldValue = Utility.separateHourAndMinute(fieldValue)
            this.setState({
                data: { ...this.state.data, [fieldName]: fieldValue },
            })
        } else {
            this.setState({
                data: { ...this.state.data, [fieldName]: fieldValue },
            })
        }
    }

    async handleSubmit (event) {
        event.preventDefault()
        this.setState({
            isLoading: true,
        })
        await this.configSet()
    }

    async handleWifiScan (event) {
        event.preventDefault()
        this.setState({
            isLoading: true,
        })
        await this.wifiScan()
    }

    async handleWifiForget (event) {
        event.preventDefault()
        this.setState({
            isLoading: false,
        })
        await this.wifiForget()
    }

    toggleExpertMode () {
        this.setState({
            showExpertMode: !this.state.showExpertMode,
        })
    }

    onScroll () {
        this.setState({
            showScrollToTop: (document.documentElement.scrollTop > 300),
        })
    }

    render () {
        return (
            <div id="top" className="container w-full flex flex-wrap mx-auto">
                <Router>
                    <Routes>
                        <Route exact path="/" element={
                            this.state.wifi.captivePortal ? (
                                <div className="w-full lg:w-3/5 mx-auto pb-12 mt-16 lg:mt-36">
                                    <Captive data={this.state.data} networks={this.state.networks} onWifiScan={this.handleWifiScan.bind(this)} onChange={this.handleChange.bind(this)} onSubmit={this.wifiSet.bind(this)}/>
                                </div>
                            ) : (
                                <>
                                    <div className="w-full lg:w-1/5 px-6">
                                        <Sidebar showExpertMode={this.state.showExpertMode} onToggleExpertMode={this.toggleExpertMode.bind(this)}/>
                                    </div>
                                    <div className="w-full lg:w-3/5 pb-12 mt-16 lg:mt-36">
                                        <ColorAndBrightness data={this.state.data} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)}/>
                                        <ModeAndLanguage data={this.state.data} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)}/>
                                        {this.state.showExpertMode && (
                                            <TimeSettings data={this.state.data} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)}/>
                                        )}
                                        <OnOffTime data={this.state.data} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)}/>
                                        {this.state.showExpertMode && (
                                            <>
                                                <Network data={this.state.data} wifi={this.state.wifi} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)} onWifiForget={this.handleWifiForget.bind(this)}/>
                                                <Accessibility data={this.state.data} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)}/>
                                                <Security data={this.state.data} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)}/>
                                                {/*}
                                                <Firmware/>
                                                {*/}
                                                <System info={this.state.info} onSystemRestart={this.systemRestart.bind(this)}/>
                                                <Licences/>
                                            </>
                                        )}
                                    </div>
                                </>
                            )
                        } />
                    </Routes>
                </Router>
                <Link to="top" smooth className={`link-to-top ${!this.state.showScrollToTop ? 'hidden' : ''}`}>
                    <ChevronUp className="h-5 w-5"/>
                </Link>
                <Loader isBusy={this.state.isLoading}/>
            </div>
        )
    }
}
