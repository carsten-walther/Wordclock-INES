import React from 'react'

export default class Accessibility extends React.Component {
    render () {
        return (
            <div id="accessibility" className="card">
                <h2 className="card-header">
                    Accessibility <button type="button" className="float-right form-btn-green" onClick={this.props.onSubmit.bind(this)}>Save</button>
                </h2>
                <div className="card-body">
                    <div className="md:flex">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="hostname">Hostname</label>
                        </div>
                        <div className="md:w-2/3">
                            <div className="flex rounded-md shadow-sm">
                                <span className="inline-flex items-center px-3 rounded-l-md border border-r-0 border-gray-300 bg-gray-50 text-gray-500 text-sm">http://</span>
                                <input type="text" name="hostname" id="hostname" className="form-input rounded-none" value={this.props.data.hostname} onChange={this.props.onChange.bind(this)} />
                                <span className="inline-flex items-center px-3 rounded-r-md border border-l-0 border-gray-300 bg-gray-50 text-gray-500 text-sm">.local</span>
                            </div>
                            <p className="help-text">Enter the host name under which WordClock should be accessible. Changing the host name requires a reset. (.local will be added automatically)</p>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
