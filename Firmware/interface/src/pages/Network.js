import React from 'react'

export default class Network extends React.Component {

    constructor (props) {
        super(props)
        this.state = {
            static: false,
        }
    }

    toggleStatic () {
        this.setState({
            static: !this.state.static,
        })
    }

    render () {
        return (
            <div id="network" className="card">
                <h2 className="card-header">
                    Network <button type="button" className="float-right form-btn-green " onClick={this.props.onSubmit.bind(this)}>Save</button>
                </h2>
                <div className="card-body">
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label">Captive Mode</label>
                        </div>
                        <div className="md:w-2/3">
                            <button type="button" className="form-btn-red " onClick={this.props.onWifiForget.bind(this)}>Reset WiFi</button>
                            <p className="help-text">To set the clock to the capitve mode, press "Reset network". Finally, the clock resets.</p>
                        </div>
                    </div>
                    <hr className="my-6"/>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="useStatic">Static settings</label>
                        </div>
                        <div className="md:w-2/3">
                            <div>
                                <div className="relative inline-block w-10 mr-2 align-middle select-none transition duration-200 ease-in">
                                    <input type="checkbox" name="static" id="static" className="toggle-checkbox" checked={this.state.static} onChange={this.toggleStatic.bind(this)}/>
                                    <label htmlFor="static" className="toggle-label block overflow-hidden h-6 rounded-full bg-gray-300 cursor-pointer"/>
                                </div>
                                <label htmlFor="static" className="inline-flex items-center">
                                    <span className="ml-2">Use static network settings</span>
                                </label>
                            </div>
                            <p className="help-text">Use static network settings.</p>
                        </div>
                    </div>
                    {this.state.static && (
                        <>
                            <div className="md:flex mb-6">
                                <div className="md:w-1/3">
                                    <label className="form-label" htmlFor="ip">IP Address</label>
                                </div>
                                <div className="md:w-2/3">
                                    <input type="text" name="ip" id="ip" className="form-input" value={this.props.wifi.ip} onChange={this.props.onChange.bind(this)}/>
                                    <p className="help-text">Enter the static IP address.</p>
                                </div>
                            </div>
                            <div className="md:flex mb-6">
                                <div className="md:w-1/3">
                                    <label className="form-label" htmlFor="sub">Subnet mask</label>
                                </div>
                                <div className="md:w-2/3">
                                    <input type="text" name="sub" id="sub" className="form-input" value={this.props.wifi.sub} onChange={this.props.onChange.bind(this)}/>
                                    <p className="help-text">Enter the Subnet mask.</p>
                                </div>
                            </div>
                            <div className="md:flex mb-6">
                                <div className="md:w-1/3">
                                    <label className="form-label" htmlFor="gw">Gateway</label>
                                </div>
                                <div className="md:w-2/3">
                                    <input type="text" name="gw" id="gw" className="form-input" value={this.props.wifi.gw} onChange={this.props.onChange.bind(this)}/>
                                    <p className="help-text">Enter the Gateway.</p>
                                </div>
                            </div>
                            <div className="md:flex mb-6">
                                <div className="md:w-1/3">
                                    <label className="form-label" htmlFor="dns">DNS Server</label>
                                </div>
                                <div className="md:w-2/3">
                                    <input type="text" name="dns" id="dns" className="form-input" value={this.props.wifi.gw} onChange={this.props.onChange.bind(this)}/>
                                    <p className="help-text">Enter the DNS server.</p>
                                </div>
                            </div>
                        </>
                    )}
                </div>
            </div>
        )
    }
}
