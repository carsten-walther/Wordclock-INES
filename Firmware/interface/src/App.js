import React from 'react'
import { Link } from 'react-scroll'
import { BrowserRouter, Route, Switch } from 'react-router-dom'

import Captive from './pages/Captive'
import ColorAndBrightness from './pages/ColorAndBrightness'
import ModeAndLanguage from './pages/ModeAndLanguage'
import TimeSettings from './pages/TimeSettings'
import OnOffTime from './pages/OnOffTime'
import Network from './pages/Network'
import Accessibility from './pages/Accessibility'
import Security from './pages/Security'
import Firmware from './pages/Firmware'
import System from './pages/System'
import Licences from './pages/Licences'

import Api from './utilities/Api'
import Color from './utilities/Color'
import Utility from './utilities/Utility'

import Sidebar from './components/Sidebar'
import Loader from './components/Loader'

import ChevronUp from './components/icons/ChevronUp'

export default class App extends React.Component {

    constructor(props) {
        super(props)
        this.state = {
            isCaptivePortal: false,
            showScrollToTop: false,
            isLoading: true,
            // data
            file: null,
            data: {
                // dashboard
                foreground: {
                    r: 0,
                    g: 0,
                    b: 0
                },
                background: {
                    r: 0,
                    g: 0,
                    b: 0
                },
                brightness: 0,
                mode: 0,
                language: 0,
                // time
                ntpServer: '',
                ntpSyncInterval: 0,
                timezone: 0,
                daylightSavingTime: false,
                sleep: {
                    h: 0,
                    m: 0
                },
                wakeup: {
                    h: 0,
                    m: 0
                },
                // network
                ssid: '',
                pass: '',
                hostname: '',
                useAuth: false,
                authUsername: '',
                authPassword: ''
            },
            info: {
                version: '',
                chipId: '',
                sdkVersion: '',
                cpuFreqMHz: 0,
                flashChipSpeed: 0,
                flashChipSize: 0,
                freeHeap: 0,
                mac: ''
            },
            networks: []
        }
    }

    async componentDidMount() {
        document.addEventListener('scroll', this.onScroll.bind(this))
        await this.getWifi()
        await this.getConfig()
        await this.getInfo()
        await this.scanWifi()
    }

    async getConfig() {
        await Api.getConfig().then(result => {
            this.setState({
                isLoading: false,
                data: result.payload
            })
        })
    }

    async setConfig() {
        let returnVal = []

        Object.keys(this.state.data).map((key) => {
            if (key === 'foreground') {
                returnVal['foregroundRed'] = this.state.data[key].r
                returnVal['foregroundGreen'] = this.state.data[key].g
                returnVal['foregroundBlue'] = this.state.data[key].b
            }
            else if (key === 'background') {
                returnVal['backgroundRed'] = this.state.data[key].r
                returnVal['backgroundGreen'] = this.state.data[key].g
                returnVal['backgroundBlue'] = this.state.data[key].b
            }
            else if (key === 'sleep') {
                returnVal['sleepHour'] = this.state.data[key].h
                returnVal['sleepMinute'] = this.state.data[key].m
            }
            else if (key === 'wakeup') {
                returnVal['wakeupHour'] = this.state.data[key].h
                returnVal['wakeupMinute'] = this.state.data[key].m
            }
            else {
                returnVal[key] = this.state.data[key]
            }
            return null
        })

        await Api.setConfig(returnVal).then(async (result) => {
            this.setState({
                isLoading: false
            })
            await this.getConfig()
        })
    }

    async getInfo() {
        await Api.info().then((result) => {
            this.setState({
                isLoading: false,
                info: { ...result.payload }
            })
        })
    }

    async getWifi() {
        await Api.getWifi().then((result) => {
            this.setState({
                isLoading: false,
                isCaptivePortal: result.payload.captivePortal
            })
        })
    }

    async scanWifi() {
        await Api.scanWifi().then((result) => {
            this.setState({
                isLoading: false,
                networks: result.payload.networks
            })
        })
    }

    async resetWifi() {
        await Api.resetWifi().then((result) => {
            this.setState({
                isLoading: true
            })
        })
    }

    async handleChange(event) {
        let fieldName = event.target.name ? event.target.name : event.target.id
        let fieldValue = null
        switch (event.target.type) {
            default:
                fieldValue = event.target.value
                break
            case "file":
                fieldValue = event.target.files[0]
                break
            case "checkbox":
                fieldValue = event.target.checked ? "true" : "false"
                break
            case "radio":
                fieldValue = event.target.value
                break
        }

        if (fieldName === 'foreground' || fieldName === 'background') {
            fieldValue = Color.hex2rgb(fieldValue)
            this.setState({
                data: { ...this.state.data, [fieldName]: fieldValue }
            })
        } else if (fieldName === 'sleep' || fieldName === 'wakeup') {
            fieldValue = Utility.separateHourAndMinute(fieldValue)
            this.setState({
                data: { ...this.state.data, [fieldName]: fieldValue }
            })
        } else {
            this.setState({
                data: { ...this.state.data, [fieldName]: fieldValue }
            })
        }
    }

    async handleSubmit(event) {
        event.preventDefault()
        this.setState({
            isLoading: true
        })
        await this.setConfig()
    }

    async handleNetworkScan(event) {
        event.preventDefault()
        this.setState({
            isLoading: true
        })
        await this.scanWifi()
    }

    async handleNetworkReset(event) {
        event.preventDefault()
        this.setState({
            isLoading: false
        })
        await this.resetWifi()
    }

    onScroll() {
        this.setState({
            showScrollToTop: (document.documentElement.scrollTop > 300)
        })
    }

    render() {
        return (
            <div id="top" className="container w-full flex flex-wrap mx-auto">
                <BrowserRouter>
                    <Switch>
                        <Route>
                            <Route path="/" exact>
                                {this.state.isCaptivePortal ? (
                                    <>
                                        <div className="w-full lg:w-3/5 mx-auto pb-12 mt-16 lg:mt-36">
                                            <Captive data={this.state.data} />
                                        </div>
                                    </>
                                ) : (
                                    <>
                                        <div className="w-full lg:w-1/5 px-6">
                                            <Sidebar/>
                                        </div>
                                        <div className="w-full lg:w-3/5 pb-12 mt-16 lg:mt-36">
                                            <ColorAndBrightness data={this.state.data} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)}/>
                                            <ModeAndLanguage data={this.state.data} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)}/>
                                            <TimeSettings data={this.state.data} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)}/>
                                            <OnOffTime data={this.state.data} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)}/>
                                            <Network data={this.state.data} networks={this.state.networks} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)} onNetworkScan={this.handleNetworkScan.bind(this)} onNetworkReset={this.handleNetworkReset.bind(this)}/>
                                            <Accessibility data={this.state.data} networks={this.state.networks} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)} onNetworkScan={this.handleNetworkScan.bind(this)} onNetworkReset={this.handleNetworkReset.bind(this)}/>
                                            <Security data={this.state.data} networks={this.state.networks} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)} onNetworkScan={this.handleNetworkScan.bind(this)} onNetworkReset={this.handleNetworkReset.bind(this)}/>
                                            <Firmware/>
                                            <System info={this.state.info}/>
                                            <Licences/>
                                        </div>
                                    </>
                                )}
                            </Route>
                        </Route>
                    </Switch>
                </BrowserRouter>
                <Link to="top" smooth className={`link-to-top ${!this.state.showScrollToTop ? 'hidden' : ''}`}>
                    <ChevronUp className="h-5 w-5"/>
                </Link>
                <Loader isBusy={this.state.isLoading}/>
            </div>
        )
    }
}
