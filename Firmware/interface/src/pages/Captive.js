import React from 'react'

import Backdrop from '../components/Backdrop'
import ChevronDown from '../components/icons/ChevronDown'
import Close from '../components/icons/Close'

export default class Captive extends React.Component {

    constructor(props) {
        super(props)
        this.state = {
            modalShow: false,
            gtcAccepted: false
        }
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
                <h2 className="card-header">
                    WiFi Settings
                </h2>
                <div className="card-body">
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="ssid">
                                SSID
                            </label>
                        </div>
                        <div className="md:w-2/3">
                            <div className="relative">
                                <select id="ssid" className="form-input" onChange={this.props.onChange.bind(this)}>
                                    <option value="">SSID</option>
                                    {this.props.networks && this.props.networks.map((network, index) => (
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
                            <label className="form-label" htmlFor="pass">
                                Password
                            </label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="password" name="pass" id="pass" className="form-input" placeholder="Password" onChange={this.props.onChange.bind(this)}/>
                        </div>
                    </div>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3"/>
                        <div className="md:w-2/3">
                            <button type="button" className="form-btn-green cursor-pointer" onClick={this.props.onNetworkScan.bind(this)}>
                                Scan networks
                            </button>
                        </div>
                    </div>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3"/>
                        <div className="md:w-2/3">
                            <label className="flex items-center">
                                <input type="checkbox" name="gtc" id="gtc" className="form-checkbox" checked={this.state.gtcAccepted} onChange={this.handleGTC.bind(this)}/>
                                <span className="ml-2">I accept the <span className="cursor-pointer underline" onClick={this.handleToggleModal.bind(this)}>general terms and conditions</span>.</span>
                            </label>
                        </div>
                    </div>
                    <div className="md:flex md:items-center">
                        <div className="md:w-1/3"/>
                        <div className="md:w-2/3">
                            <button type="submit" className="form-btn-red" disabled={!this.state.gtcAccepted || !this.props.data.ssid.length > 0} onClick={this.props.onSubmit.bind(this)}>
                                Save
                            </button>
                        </div>
                    </div>
                </div>
                {this.state.modalShow && (
                    <Backdrop>
                        <div className="w-full lg:w-3/5 mx-auto pb-12 mt-16 lg:mt-36">
                            <div className="card">
                                <h2 className="card-header">
                                    General Terms and Conditions
                                    <button type="button" className="float-right  py-1" onClick={this.handleToggleModal.bind(this)}>
                                        <Close className="h-5 w-5" />
                                    </button>
                                </h2>
                                <div className="card-body py-8 h-96 overscroll-contain overflow-auto">
                                    <p>...</p>
                                </div>
                            </div>
                        </div>
                    </Backdrop>
                )}
            </div>
        )
    }
}
