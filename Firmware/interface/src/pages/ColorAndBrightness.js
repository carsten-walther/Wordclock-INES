import React from 'react'

import ColorPicker from '../components/ColorPicker'
import Color from '../utilities/Color'

export default class ColorAndBrightness extends React.Component {
    render () {
        return (
            <div id="color-and-brightness" className="card">
                <h2 className="card-header">
                    Colors & Brightness <button type="button" className="float-right form-btn-green " onClick={this.props.onSubmit.bind(this)}>Save</button>
                </h2>
                <div className="card-body">
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="foreground">Foreground color</label>
                        </div>
                        <div className="md:w-2/3">
                            <ColorPicker name="foreground" id="foreground" className="form-input" selectedColor={Color.rgb2hex(this.props.data.foreground.r, this.props.data.foreground.g, this.props.data.foreground.b)} onChange={this.props.onChange.bind(this)}/>
                            <p className="help-text">Specify the color for the highlighted words.</p>
                        </div>
                    </div>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="background">Background color</label>
                        </div>
                        <div className="md:w-2/3">
                            <ColorPicker name="background" id="background" className="form-input" selectedColor={Color.rgb2hex(this.props.data.background.r, this.props.data.background.g, this.props.data.background.b)} onChange={this.props.onChange.bind(this)}/>
                            <p className="help-text">Specify the background color.</p>
                        </div>
                    </div>
                    <div className="md:flex">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="brightness">Brightness</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="range" min="0" max="100" step="1" name="brightness" id="brightness" className="w-full cursor-ew-resize" value={this.props.data.brightness} onChange={this.props.onChange.bind(this)}/>
                            <p className="help-text">Set the desired brightness.</p>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
