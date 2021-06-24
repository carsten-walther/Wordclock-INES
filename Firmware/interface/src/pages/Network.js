import React from 'react'

import ChevronDown from '../components/icons/ChevronDown'

export default class Network extends React.Component {
    render () {
        return (
            <div className="card">
                <h2 className="card-header">Network</h2>
                <div className="card-text">
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="ssid">SSID</label>
                        </div>
                        <div className="md:w-2/3">
                            <div className="relative">
                                <select name="ssid" id="ssid" value={this.props.data.ssid} className="form-input" onChange={this.props.onChange.bind(this)}>
                                    <option value="">SSID</option>
                                    {this.props.networks && this.props.networks.map((network, index) => (
                                        <option value={network.ssid} key={index}>
                                            {(network.encryptionType > 0) ? '🔒 ' : '🔓 '} {network.ssid} {(network.encryptionType > 0) ? '(closed)' : '(open)'}
                                        </option>
                                    ))}
                                </select>
                                <div className="pointer-events-none absolute inset-y-0 right-0 flex items-center px-2 text-gray-700">
                                    <ChevronDown className="h-5 w-5 float-right" />
                                </div>
                            </div>
                            <p className="help-text">Please select your preferred WLAN name.</p>
                        </div>
                    </div>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="pass">Password</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="password" name="pass" id="pass" className="appearance-none block leading-none w-full text-gray-700 border border-gray-300 rounded py-3 px-4" value={this.props.data.pass} onChange={this.props.onChange.bind(this)} />
                            <p className="help-text">Please enter your WLAN password.</p>
                        </div>
                    </div>
                    <div className="md:flex md:items-center">
                        <div className="md:w-1/3" />
                        <div className="md:w-2/3">
                            <div className="flex">
                                <div className="w-1/2">
                                    <button type="button" className="float-left form-btn-green" onClick={this.props.onNetworkScan.bind(this)}>Scan networks</button>
                                </div>
                                <div className="w-1/2">
                                    <button type="button" className="float-right form-btn-red" onClick={this.props.onNetworkReset.bind(this)}>Reset network</button>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
