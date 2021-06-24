import React from 'react'
import { Link } from 'react-scroll'
import { BrowserRouter, Route, Switch } from 'react-router-dom'

import Sidebar from './components/Sidebar'
import Divider from './components/Divider'
import Loader from './components/Loader'

import Dashboard from './pages/Dashboard'
import Time from './pages/Time'
import Network from './pages/Network'
import Firmware from './pages/Firmware'
import System from './pages/System'
import Licences from './pages/Licences'

import Api from './utilities/Api'
import Color from './utilities/Color'
import ChevronUp from './components/icons/ChevronUp'
import Utility from './utilities/Utility'

export default class App extends React.Component {

    constructor (props) {
        super(props)
        this.state = {
            showScrollToTop: false,
            isLoading: true,
            // data
            file: null,
            data: {
                // dashboard
                foregroundColor: {
                    r: 0,
                    g: 0,
                    b: 0
                },
                backgroundColor: {
                    r: 0,
                    g: 0,
                    b: 0
                },
                brightness: 0,
                clockMode: 0,
                language: 0,
                // time
                ntpServer: '',
                ntpSyncInterval: 0,
                timezone: 0,
                daylightSavingTime: false,
                sleepHour: 0,
                sleepMinute: 0,
                wakeupHour: 0,
                wakeupMinute: 0,
                sleepTime: '00:00',
                wakeupTime: '06:00',
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

    async componentDidMount () {
        document.addEventListener('scroll', this.onScroll.bind(this))
        await this.getConfig()
        await this.getInfo()
        await this.scanWifi()
    }

    async getConfig () {
        await Api.getConfig().then(result => {
            let data = {}
            for (const [key, value] of Object.entries(result.payload)) {

                let dataResult = value
                if (key === 'foregroundColor' || key === 'backgroundColor') {
                    dataResult = Color.rgb2hex(value.r, value.g, value.b)
                    data = { ...data, [key]: dataResult }
                }
                else if (key === 'sleepHour' || key === 'sleepMinute') {
                    dataResult = Utility.combineHourAndMinute(result.payload.sleepHour, result.payload.sleepMinute)
                    data = { ...data, 'sleepTime': dataResult }
                }
                else if (key === 'wakeupHour' || key === 'wakeupMinute') {
                    dataResult = Utility.combineHourAndMinute(result.payload.wakeupHour, result.payload.wakeupMinute)
                    data = { ...data, 'wakeupTime': dataResult }
                }
                else {
                    data = { ...data, [key]: dataResult }
                }
            }
            this.setState({
                isLoading: false,
                data: data
            })
        })
    }

    async setConfig () {
        await Api.setConfig(this.state.data).then(async (result) => {
            this.setState({
                isLoading: false
            })
            await this.getConfig()
        })
    }

    async getInfo () {
        await Api.info().then((result) => {
            this.setState({
                isLoading: false,
                info: { ...result.payload }
            })
        })
    }

    async scanWifi () {
        await Api.scanWifi().then((result) => {
            this.setState({
                isLoading: false,
                networks: result.payload.networks
            })
        })
    }

    async resetWifi () {
        await Api.resetWifi().then((result) => {
            this.setState({
                isLoading: true
            })
        })
    }

    handleChange (event) {
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
        }

        if (fieldName === 'foregroundColor' || fieldName === 'backgroundColor') {
            fieldValue = Color.hex2rgb(fieldValue)
            this.setState({
                data: { ...this.state.data, [fieldName]: fieldValue }
            })
        } else if (fieldName === 'sleepTime') {
            this.setState({
                data: { ...this.state.data, [fieldName]: fieldValue }
            })
            let sleepTime = Utility.separateHourAndMinute(fieldValue)
            fieldName = 'sleepHour'
            fieldValue = sleepTime[0]
            this.setState({
                data: { ...this.state.data, [fieldName]: fieldValue }
            })
            fieldName = 'sleepMinute'
            fieldValue = sleepTime[1]
            this.setState({
                data: { ...this.state.data, [fieldName]: fieldValue }
            })
        } else if (fieldName === 'wakeupTime') {
            this.setState({
                data: { ...this.state.data, [fieldName]: fieldValue }
            })
            let wakeupTime = Utility.separateHourAndMinute(fieldValue)
            fieldName = 'wakeupHour'
            fieldValue = wakeupTime[0]
            this.setState({
                data: { ...this.state.data, [fieldName]: fieldValue }
            })
            fieldName = 'wakeupMinute'
            fieldValue = wakeupTime[1]
            this.setState({
                data: { ...this.state.data, [fieldName]: fieldValue }
            })
        } else {
            this.setState({
                data: { ...this.state.data, [fieldName]: fieldValue }
            })
        }

        console.log(fieldName, fieldValue)
    }

    async handleSubmit (event) {
        event.preventDefault()
        this.setState({
            isLoading: true
        })
        await this.setConfig()
    }

    async handleNetworkScan (event) {
        event.preventDefault()
        this.setState({
            isLoading: true
        })
        await this.scanWifi()
    }

    async handleNetworkReset (event) {
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

    render () {
        return (
            <BrowserRouter>
                <div id="top" className="container w-full flex flex-wrap mx-auto px-2 pt-8 lg:pt-16 mt-0 lg:mt-16">
                    <Sidebar />
                    <section className="w-full lg:w-3/5 mt-1 pb-12">
                        <Switch>
                            <Route>
                                <Route path="/" exact>
                                    <div id="dashboard">
                                        <Dashboard data={this.state.data} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)} />
                                        <Divider />
                                    </div>
                                    <div id="time">
                                        <Time data={this.state.data} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)} />
                                        <Divider />
                                    </div>
                                    <div id="network">
                                        <Network data={this.state.data} networks={this.state.networks} onChange={this.handleChange.bind(this)} onSubmit={this.handleSubmit.bind(this)} onNetworkScan={this.handleNetworkScan.bind(this)} onNetworkReset={this.handleNetworkReset.bind(this)} />
                                        <Divider />
                                    </div>
                                    <div id="firmware">
                                        <Firmware />
                                        <Divider />
                                    </div>
                                    <div id="system">
                                        <System info={this.state.info} />
                                        <Divider />
                                    </div>
                                    <div id="licences">
                                        <Licences />
                                    </div>
                                </Route>
                            </Route>
                        </Switch>
                    </section>
                </div>
                <Link to="top" smooth className={`fixed bottom-4 right-4 rounded-full h-10 w-10 flex items-center justify-center shadow-lg z-50 bg-gray-700 text-white cursor-pointer ${!this.state.showScrollToTop ? 'hidden' : ''}`}>
                    <ChevronUp className="h-5 w-5" />
                </Link>
                <Loader isBusy={this.state.isLoading}/>
            </BrowserRouter>
        )
    }
}
