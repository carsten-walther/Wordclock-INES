import React from 'react'

export default class Security extends React.Component {
    render () {
        return (
            <div id="security" className="card">
                <h2 className="card-header">
                    Security
                    <button type="button" className="float-right form-btn-red text-sm py-1 px-2" onClick={this.props.onSubmit.bind(this)}>Save</button>
                </h2>
                <div className="card-body">
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
                            <p className="help-text">To protect the clock from unwanted manipulation you can activate basic authentication.</p>
                        </div>
                    </div>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="authUsername">Username</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="text" name="authUsername" id="authUsername" className="appearance-none block leading-none w-full text-gray-700 border border-gray-300 rounded py-3 px-4" value={this.props.data.authUsername} onChange={this.props.onChange.bind(this)} disabled={!this.props.data.useAuth} />
                            <p className="help-text">Enter a username for authentication.</p>
                        </div>
                    </div>
                    <div className="md:flex">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="authPassword">Password</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="password" name="authPassword" id="authPassword" className="appearance-none block leading-none w-full text-gray-700 border border-gray-300 rounded py-3 px-4" value={this.props.data.authPassword} onChange={this.props.onChange.bind(this)} disabled={!this.props.data.useAuth} />
                            <p className="help-text">Enter a password for authentication.</p>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
