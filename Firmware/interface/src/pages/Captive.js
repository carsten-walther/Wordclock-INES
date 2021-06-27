import React from 'react'

import ChevronDown from '../components/icons/ChevronDown'

export default class Captive extends React.Component {
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
                            <button type="button" className="form-btn-green cursor-pointer" onClick={this.props.onWifiScan.bind(this)}>
                                Scan networks
                            </button>
                        </div>
                    </div>
                    <div className="md:flex md:items-center">
                        <div className="md:w-1/3"/>
                        <div className="md:w-2/3">
                            <button type="submit" className="form-btn-red" disabled={!this.props.data.ssid.length > 0} onClick={this.props.onSubmit.bind(this)}>
                                Save
                            </button>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
