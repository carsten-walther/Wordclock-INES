import React from 'react'

import ColorPicker from '../components/ColorPicker'

export default class ColorAndBrightness extends React.Component {
    render () {
        return (
            <div className="card">
                <h2 className="card-header">Colors & Brightness</h2>
                <div className="card-text">
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="foregroundColor">Foreground color</label>
                        </div>
                        <div className="md:w-2/3">
                            <ColorPicker name="foregroundColor" id="foregroundColor" className="form-input" selectedColor={this.props.data.foregroundColor} onChange={this.props.onChange.bind(this)} />
                            <p className="help-text">Please select the color for the highlighted words.</p>
                        </div>
                    </div>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="backgroundColor">Background color</label>
                        </div>
                        <div className="md:w-2/3">
                            <ColorPicker name="backgroundColor" id="backgroundColor" className="form-input" selectedColor={this.props.data.backgroundColor} onChange={this.props.onChange.bind(this)} />
                            <p className="help-text">Please select the color for the background.</p>
                        </div>
                    </div>
                    <div className="md:flex">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="brightness">Brightness</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="range" min="0" max="100" step="1" name="brightness" id="brightness" className="w-full cursor-ew-resize" value={this.props.data.brightness} onChange={this.props.onChange.bind(this)} />
                            <p className="help-text">Please select the brightness for the clock.</p>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
