import React from 'react'

import ChevronDown from '../components/icons/ChevronDown'

export default class Security extends React.Component {
    render () {
        return (
            <div className="card">
                <h2 className="card-header">Security</h2>
                <div className="card-text">
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="useAuth">Use Authentication</label>
                        </div>
                        <div className="md:w-2/3">
                            <div>
                                <label className="inline-flex items-center">
                                    <input type="checkbox" name="useAuth" id="useAuth" className="form-checkbox text-indigo-600" checked={this.props.data.useAuth} onChange={this.props.onChange.bind(this)} />
                                    <span className="ml-2">Use basic authentication for access.</span>
                                </label>
                            </div>
                            <p className="help-text">Would you like to active a basic authentication?</p>
                        </div>
                    </div>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="authUsername">Username</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="text" name="authUsername" id="authUsername" className="appearance-none block leading-none w-full text-gray-700 border border-gray-300 rounded py-3 px-4" value={this.props.data.authUsername} onChange={this.props.onChange.bind(this)} />
                            <p className="help-text">Choose an username for authentication.</p>
                        </div>
                    </div>
                    <div className="md:flex">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="authPassword">Password</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="password" name="authPassword" id="authPassword" className="appearance-none block leading-none w-full text-gray-700 border border-gray-300 rounded py-3 px-4" value={this.props.data.authPassword} onChange={this.props.onChange.bind(this)} />
                            <p className="help-text">Enter the password for authentication.</p>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
