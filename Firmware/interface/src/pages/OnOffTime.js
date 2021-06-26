import React from 'react'
import Utility from '../utilities/Utility'

export default class OnOffTime extends React.Component {
    render () {
        return (
            <div id="on-off-time" className="card">
                <h2 className="card-header">
                    On/Off Time
                    <button type="button" className="float-right form-btn-red text-sm py-1 px-2" onClick={this.props.onSubmit.bind(this)}>Save</button>
                </h2>
                <div className="card-body">
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="sleep">Sleep Time</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="time" name="sleep" id="sleep" className="form-input" placeholder="02:00" value={Utility.combineHourAndMinute(this.props.data.sleep.h, this.props.data.sleep.m)} onChange={this.props.onChange.bind(this)} />
                            <p className="help-text">Specify the time from which the clock is to be switched to sleep mode. The display is deactivated from this time.</p>
                        </div>
                    </div>
                    <div className="md:flex">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="wakeup">Wakeup Time</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="time" name="wakeup" id="wakeup" className="form-input" placeholder="06:00" value={Utility.combineHourAndMinute(this.props.data.wakeup.h, this.props.data.wakeup.m)} onChange={this.props.onChange.bind(this)} />
                            <p className="help-text">Specify the time from which the clock is to be reactivated.</p>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
