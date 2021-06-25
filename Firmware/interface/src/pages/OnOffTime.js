import React from 'react'

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
                            <label className="form-label" htmlFor="sleepTime">Sleep Time</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="time" name="sleepTime" id="sleepTime" className="form-input" placeholder="02:00" value={this.props.data.sleepTime} onChange={this.props.onChange.bind(this)} />
                            <p className="help-text">When should wordclock go to sleep?</p>
                        </div>
                    </div>
                    <div className="md:flex">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="wakeupTime">Wakeup Time</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="time" name="wakeupTime" id="wakeupTime" className="form-input" placeholder="06:00" value={this.props.data.wakeupTime} onChange={this.props.onChange.bind(this)} />
                            <p className="help-text">When should wordclock go to wakeup?</p>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
