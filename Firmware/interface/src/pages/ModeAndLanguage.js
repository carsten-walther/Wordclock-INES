import React from 'react'

import ChevronDown from '../components/icons/ChevronDown'

export default class Dashboard extends React.Component {
    render () {
        return (
            <div id="mode-and-language" className="card">
                <h2 className="card-header">
                    Mode & Language
                    <button type="button" className="float-right form-btn-red text-sm py-1 px-2" onClick={this.props.onSubmit.bind(this)}>Save</button>
                </h2>
                <div className="card-body">
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="mode">Mode</label>
                        </div>
                        <div className="md:w-2/3">
                            <div className="mt-2">
                                <label className="form-label-inline" htmlFor="mode-1">
                                    <input type="radio" name="mode" id="mode-1" value={1} className="form-radio text-indigo-600" checked={parseInt(this.props.data.mode) === 1} onChange={this.props.onChange.bind(this)}/>
                                    <span className="ml-2">Normal</span>
                                </label>
                                <label className="form-label-inline" htmlFor="mode-2">
                                    <input type="radio" name="mode" id="mode-2" value={2} className="form-radio text-indigo-600" checked={parseInt(this.props.data.mode) === 2} onChange={this.props.onChange.bind(this)}/>
                                    <span className="ml-2">Scanner</span>
                                </label>
                                <label className="form-label-inline" htmlFor="mode-3">
                                    <input type="radio" name="mode" id="mode-3" value={3} className="form-radio text-indigo-600" checked={parseInt(this.props.data.mode) === 3} onChange={this.props.onChange.bind(this)}/>
                                    <span className="ml-2">Test</span>
                                </label>
                            </div>
                            <p className="help-text">Set the desired display mode. Normal corresponds to the display of the time, Scanner displays an animation. Test mode makes all words light up.</p>
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
                            <p className="help-text">Set the desired language/dialect of the word clock.</p>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
