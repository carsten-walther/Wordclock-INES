import React from 'react'

import Api from '../utilities/Api'
import Loader from '../components/Loader'
import ChevronDown from '../components/icons/ChevronDown'

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
            <div className="card">
                <h2 className="card-header">WiFi Settings</h2>
                <div className="card-body">
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="ssid">SSID</label>
                        </div>
                        <div className="md:w-2/3">
                            <div className="relative">
                                <select id="ssid" className="form-input" onChange={this.handleChange.bind(this)}>
                                    <option value="">SSID</option>
                                    {this.state.networks && this.state.networks.map((network, index) => (
                                        <option value={network.ssid} key={index}>
                                            {(network.encryptionType > 0) ? '🔒 ' : '🔓 '} {network.ssid} {(network.encryptionType > 0) ? '(closed)' : '(open)'}
                                        </option>
                                    ))}
                                </select>
                                <div className="form-input-chevron">
                                    <ChevronDown className="h-4 w-4"/>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="pass">Password</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="password" name="pass" id="pass" className="form-input" placeholder="Password" onChange={this.handleChange.bind(this)}/>
                        </div>
                    </div>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3"/>
                        <div className="md:w-2/3">
                            <button type="button" className="float-left text-sm cursor-pointer" onClick={this.handleScan.bind(this)}>Scan for networks!</button>
                        </div>
                    </div>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3"/>
                        <div className="md:w-2/3">
                            <label className="flex items-center">
                                <input type="checkbox" name="gtc" id="gtc" className="form-checkbox" checked={this.state.data.gtc} onChange={this.handleChange.bind(this)}/>
                                <span className="ml-2">I accept the <span className="cursor-pointer underline" onClick={this.handleToggleModal.bind(this)}>general terms and conditions</span>.</span>
                            </label>
                        </div>
                    </div>
                    <div className="md:flex md:items-center">
                        <div className="md:w-1/3"/>
                        <div className="md:w-2/3">
                            <button type="submit" className="form-btn-red" disabled={!this.state.gtcAccepted || !this.state.data.ssid.length > 0}>Save</button>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
