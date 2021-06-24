import React from 'react'

export default class Accessibility extends React.Component {
    render () {
        return (
            <div className="card">
                <h2 className="card-header">Accessibility</h2>
                <div className="card-text">
                    <div className="md:flex">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="hostname">Hostname</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="text" name="hostname" id="hostname" className="appearance-none block leading-none w-full text-gray-700 border border-gray-300 rounded py-3 px-4" value={this.props.data.hostname} onChange={this.props.onChange.bind(this)} />
                            <p className="help-text">Enter the hostname to connect to wordclock. (.local will be added automatically)</p>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
