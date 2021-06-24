import React from 'react'

import SectionHeader from '../components/SectionHeader'
import ColorPicker from '../components/ColorPicker'
import ChevronDown from '../components/icons/ChevronDown'
import Label from '../components/Form/Label'
import Color from '../utilities/Color'

export default class Dashboard extends React.Component {
    render () {
        return (
            <>
                <SectionHeader header="Colors & Brightness" />
                <div className="card">
                    <form onSubmit={this.props.onSubmit.bind(this)}>
                        <div className="md:flex mb-6">
                            <div className="md:w-1/3">
                                <Label htmlFor="foregroundColor">Foreground color</Label>
                            </div>
                            <div className="md:w-2/3">
                                <ColorPicker name="foregroundColor" id="foregroundColor" className="form-input" selectedColor={this.props.data.foregroundColor} onChange={this.props.onChange.bind(this)} />
                                <p className="help-text">Please select the color for the highlighted words.</p>
                            </div>
                        </div>
                        <div className="md:flex mb-6">
                            <div className="md:w-1/3">
                                <Label htmlFor="backgroundColor">Background color</Label>
                            </div>
                            <div className="md:w-2/3">
                                <ColorPicker name="backgroundColor" id="backgroundColor" className="form-input" selectedColor={this.props.data.backgroundColor} onChange={this.props.onChange.bind(this)} />
                                <p className="help-text">Please select the color for the background.</p>
                            </div>
                        </div>
                        <div className="md:flex mb-6">
                            <div className="md:w-1/3">
                                <Label htmlFor="brightness">Brightness</Label>
                            </div>
                            <div className="md:w-2/3">
                                <input type="range" min="0" max="100" step="1" name="brightness" id="brightness" className="rounded-lg overflow-hidden appearance-none bg-gray-300 h-3 w-full" value={this.props.data.brightness} onChange={this.props.onChange.bind(this)} />
                                <p className="help-text">Please select the brightness for the clock.</p>
                            </div>
                        </div>
                        <div className="md:flex mb-6">
                            <div className="md:w-1/3">
                                <Label htmlFor="clockMode">Mode</Label>
                            </div>
                            <div className="md:w-2/3">
                                <div className="mt-2">
                                    <label className="inline-flex items-center mr-6" htmlFor="clockMode-1">
                                        <input type="radio" name="clockMode" id="clockMode-1" value="1" className="form-radio text-indigo-600" checked={this.props.data.clockMode === 1} onChange={this.props.onChange.bind(this)}/>
                                        <span className="ml-2">Normal</span>
                                    </label>
                                    <label className="inline-flex items-center mr-6" htmlFor="clockMode-2">
                                        <input type="radio" name="clockMode" id="clockMode-2" value="2" className="form-radio text-indigo-600" checked={this.props.data.clockMode === 2} onChange={this.props.onChange.bind(this)}/>
                                        <span className="ml-2">Scanner</span>
                                    </label>
                                    <label className="inline-flex items-center mr-6" htmlFor="clockMode-3">
                                        <input type="radio" name="clockMode" id="clockMode-3" value="3" className="form-radio text-indigo-600" checked={this.props.data.clockMode === 3} onChange={this.props.onChange.bind(this)}/>
                                        <span className="ml-2">Test</span>
                                    </label>
                                </div>
                            </div>
                        </div>
                        <div className="md:flex mb-6">
                            <div className="md:w-1/3">
                                <Label htmlFor="language">Language</Label>
                            </div>
                            <div className="md:w-2/3">
                                <div className="relative">
                                    <select name="language" id="language" value={this.props.data.language} className="form-input" onChange={this.props.onChange.bind(this)}>
                                        <option key={0} value="0">Hochdeutsch</option>
                                        <option key={1} value="1">Swabian</option>
                                        <option key={2} value="2">Bavarian</option>
                                        <option key={3} value="3">Sächsisch</option>
                                    </select>
                                    <div className="pointer-events-none absolute inset-y-0 right-0 flex items-center px-2 text-gray-700">
                                        <ChevronDown className="h-5 w-5 float-right" />
                                    </div>
                                </div>
                                <p className="help-text">Please select your preferred language.</p>
                            </div>
                        </div>
                        <div className="md:flex md:items-center">
                            <div className="md:w-1/3" />
                            <div className="md:w-2/3">
                                <button type="submit" className="shadow bg-red-600 hover:bg-red-500 focus:shadow-outline focus:outline-none text-white font-bold py-2 px-4 rounded">Save</button>
                            </div>
                        </div>
                    </form>
                </div>
            </>
        )
    }
}
