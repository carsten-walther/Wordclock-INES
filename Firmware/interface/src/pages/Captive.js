import React from 'react'

import ChevronDown from '../components/icons/ChevronDown'
import Eye from '../components/icons/Eye'
import EyeOff from '../components/icons/EyeOff'

export default class Captive extends React.Component {

    constructor (props) {
        super(props)
        this.state = {
            inputType: 'password',
        }
    }

    toggleInputType () {
        this.setState({
            inputType: this.state.inputType === 'password' ? 'text' : 'password',
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
                            <label className="form-label" htmlFor="ssid">SSID</label>
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
                                    <ChevronDown className="h-5 w-5"/>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="pass">Password</label>
                        </div>
                        <div className="md:w-2/3">
                            <div className="flex rounded-md shadow-sm">
                                <input type={this.state.inputType} name="pass" id="pass" className="form-input rounded-none rounded-l-md" placeholder="Password" onChange={this.props.onChange.bind(this)}/>
                                <button type="button" className="inline-flex items-center px-3 rounded-r-md border border-l-0 border-gray-300 bg-gray-50 text-gray-500 text-sm" onClick={this.toggleInputType.bind(this)}>
                                    {this.state.inputType === 'password' ? (
                                        <Eye className="h-5 w-5"/>
                                    ) : (
                                        <EyeOff className="h-5 w-5"/>
                                    )}
                                </button>
                            </div>
                        </div>
                    </div>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3"/>
                        <div className="md:w-2/3">
                            <button type="button" className="form-btn-green " onClick={this.props.onWifiScan.bind(this)}>Scan networks</button>
                        </div>
                    </div>
                    <div className="md:flex md:items-center">
                        <div className="md:w-1/3"/>
                        <div className="md:w-2/3">
                            <button type="submit" className="form-btn-red" disabled={!this.props.data.ssid} onClick={this.props.onSubmit.bind(this)}>Save</button>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
