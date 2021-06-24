import React from 'react'

import ChevronDown from '../components/icons/ChevronDown'

export default class Dashboard extends React.Component {
    render () {
        return (
            <div className="card">
                <h2 className="card-header">Mode & Language</h2>
                <div className="card-text">
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="clockMode">Mode</label>
                        </div>
                        <div className="md:w-2/3">
                            <div className="mt-2">
                                <label className="form-label-inline" htmlFor="clockMode-1">
                                    <input type="radio" name="clockMode" id="clockMode-1" value="1" className="form-radio text-indigo-600" checked={this.props.data.clockMode === 1} onChange={this.props.onChange.bind(this)}/>
                                    <span className="ml-2">Normal</span>
                                </label>
                                <label className="form-label-inline" htmlFor="clockMode-2">
                                    <input type="radio" name="clockMode" id="clockMode-2" value="2" className="form-radio text-indigo-600" checked={this.props.data.clockMode === 2} onChange={this.props.onChange.bind(this)}/>
                                    <span className="ml-2">Scanner</span>
                                </label>
                                <label className="form-label-inline" htmlFor="clockMode-3">
                                    <input type="radio" name="clockMode" id="clockMode-3" value="3" className="form-radio text-indigo-600" checked={this.props.data.clockMode === 3} onChange={this.props.onChange.bind(this)}/>
                                    <span className="ml-2">Test</span>
                                </label>
                            </div>
                        </div>
                    </div>
                    <div className="md:flex">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="language">Language</label>
                        </div>
                        <div className="md:w-2/3">
                            <div className="relative">
                                <select name="language" id="language" value={this.props.data.language} className="form-input" onChange={this.props.onChange.bind(this)}>
                                    <option key={0} value="0">Hochdeutsch</option>
                                    <option key={1} value="1">Swabian</option>
                                    <option key={2} value="2">Bavarian</option>
                                    <option key={3} value="3">Sächsisch</option>
                                </select>
                                <div className="form-input-chevron">
                                    <ChevronDown className="h-5 w-5 float-right" />
                                </div>
                            </div>
                            <p className="help-text">Please select your preferred language.</p>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
