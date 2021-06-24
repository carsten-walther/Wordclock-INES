import React from 'react'

import Api from './utilities/Api'
import Loader from './components/Loader'
import SectionHeader from './components/SectionHeader'
import ChevronDown from './components/icons/ChevronDown'

export default class Captive extends React.Component {

    constructor(props) {
        super(props)
        this.state = {
            isLoading: true,
            modalShow: false,
            gtcAccepted: false,
            // data
            data: {
                ssid: '',
                pass: ''
            },
            networks: []
        }
    }

    async componentDidMount () {
        await this.scanWifi()
    }

    async scanWifi () {
        await Api.scanWifi().then((result) => {
            this.setState({
                isLoading: false,
                networks: result.payload.networks
            })
        })
    }

    async setWifi () {
        await Api.setWifi(this.state.data).then((result) => {
            this.setState({
                isLoading: false
            })
        })
    }

    async handleScan (event) {
        event.preventDefault()
        this.setState({
            isLoading: true
        })
        await this.scanWifi()
    }

    handleChange (event) {
        let fieldName = event.target.name ? event.target.name : event.target.id
        let fieldValue = null
        switch (event.target.type) {
            default:
                fieldValue = event.target.value
                break
            case "checkbox":
                fieldValue = event.target.checked
                break
        }
        this.setState({
            data: { ...this.state.data, [fieldName]: fieldValue }
        })
    }

    async handleSubmit (event) {
        event.preventDefault()
        this.setState({
            isLoading: true
        })
        await this.setWifi()
    }

    handleGTC (event) {
        this.setState({
            gtcAccepted: event.target.checked
        })
    }

    handleToggleModal (event) {
        event.preventDefault()
        this.setState({
            modalShow: !this.state.modalShow
        })
    }

    render () {
        return (
            <main>
                <section className="relative w-full h-full py-40 min-h-screen">
                    <div className="container mx-auto px-4 h-full">
                        <div className="flex content-center items-center justify-center h-full">
                            <div className="w-full lg:w-3/5 px-4">
                                <SectionHeader header="WiFi Settings" />
                                <div className="p-8 mt-6 lg:mt-0 rounded shadow bg-white">
                                    <form onSubmit={this.handleSubmit.bind(this)} className="user">
                                        <div className="md:flex mb-6">
                                            <div className="md:w-1/3">
                                                <label className="block text-gray-500 font-bold md:text-left mb-3 md:mb-0 pr-4" htmlFor="ssid">SSID</label>
                                            </div>
                                            <div className="md:w-2/3">
                                                <div className="relative">
                                                    <select id="ssid" className="appearance-none block w-full bg-gray-200 text-gray-700 border border-gray-300 rounded py-3 px-4 leading-tight focus:outline-none focus:bg-white focus:border-gray-500" onChange={this.handleChange.bind(this)}>
                                                        <option value="">SSID</option>
                                                        {this.state.networks && this.state.networks.map((network, index) => {
                                                            return (
                                                                <option value={network.ssid} key={index}>
                                                                    {(network.encryptionType > 0) ? '🔒 ' : '🔓 '} {network.ssid} {(network.encryptionType > 0) ? '(closed)' : '(open)'}
                                                                </option>
                                                            )
                                                        })}
                                                    </select>
                                                    <div className="pointer-events-none absolute inset-y-0 right-0 flex items-center px-2 text-gray-700">
                                                        <ChevronDown className="h-4 w-4" />
                                                    </div>
                                                </div>
                                            </div>
                                        </div>
                                        <div className="md:flex mb-6">
                                            <div className="md:w-1/3">
                                                <label className="block text-gray-500 font-bold md:text-left mb-3 md:mb-0 pr-4" htmlFor="pass">Password</label>
                                            </div>
                                            <div className="md:w-2/3">
                                                <input type="password" name="pass" id="pass" className="appearance-none block w-full bg-gray-200 text-gray-700 border border-gray-300 rounded py-3 px-4 leading-tight focus:outline-none focus:bg-white focus:border-gray-500" placeholder="Password" onChange={this.handleChange.bind(this)} />
                                            </div>
                                        </div>
                                        <div className="md:flex md:items-center mb-6">
                                            <div className="md:w-1/3" />
                                            <div className="md:w-2/3">
                                                <button type="button" className="float-left text-sm cursor-pointer" onClick={this.handleScan.bind(this)}>Scan for networks!</button>
                                            </div>
                                        </div>
                                        <div className="md:flex mb-6">
                                            <div className="md:w-1/3" />
                                            <div className="md:w-2/3">
                                                <label className="flex items-center">
                                                    <input type="checkbox" name="gtc" id="gtc" className="form-checkbox" checked={this.state.data.gtc} onChange={this.handleChange.bind(this)} />
                                                    <span className="ml-2">I accept the <a href="#gtc" className="underline" onClick={this.handleModalOpen.bind(this)}>general terms and conditions</a>.</span>
                                                </label>
                                            </div>
                                        </div>
                                        <div className="md:flex md:items-center">
                                            <div className="md:w-1/3" />
                                            <div className="md:w-2/3">
                                                <button type="submit" className="shadow bg-red-600 hover:bg-red-500 focus:shadow-outline focus:outline-none text-white font-bold py-2 px-4 rounded" disabled={!this.state.gtcAccepted || !this.state.data.ssid.length > 0}>Save</button>
                                            </div>
                                        </div>
                                    </form>
                                </div>
                            </div>
                        </div>
                    </div>
                </section>
                <Loader isBusy={this.state.isLoading}/>
            </main>
        )
    }
}
