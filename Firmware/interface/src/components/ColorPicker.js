import React from 'react'

import Picker from './icons/Picker'

export default class ColorPicker extends React.Component {

    colorPalettes = [
        {
            label: 'Default Colors',
            colors: ['#2196F3', '#009688', '#9C27B0', '#FFEB3B', '#FFFFFF', '#4CAF50', '#000000', '#f56565', '#ed64a6', '#d11141']
        }, {
            label: 'Rainbow Colors',
            colors: ['#ee4035', '#f37736', '#fdf498', '#7bc043', '#0392cf']
        }, {
            label: 'Google UI Colors',
            colors: ['#008744', '#0057e7', '#d62d20', '#ffa700', '#ffffff']
        }, {
            label: 'Metro UI Colors',
            colors: ['#d11141', '#00b159', '#00aedb', '#f37735', '#ffc425']
        }
    ]

    ref = undefined

    constructor (props) {
        super(props)
        this.state = {
            pickerOpen: false
        }
    }

    componentDidMount () {
        document.addEventListener('mousedown', this.handleClickOutside)
    }

    componentWillUnmount() {
        document.removeEventListener('mousedown', this.handleClickOutside)
    }

    handleTogglePicker() {
        this.setState({
            pickerOpen: !this.state.pickerOpen
        })
    }

    handleChange(event) {
        this.setState({
            pickerOpen: false
        })
        this.props.onChange(event)
    }

    setRef(node) {
        this.ref = node
    }

    handleClickOutside(event) {
        if (this.ref && !this.ref.current.contains(event.target)) {
            this.setState({
                pickerOpen: false
            })
        }
    }

    render() {
        return(
            <div className="w-full flex" ref={this.setRef.bind(this)}>
                <div className="w-full">
                    <input id={this.props.id} type="text" placeholder="Pick a color" className={this.props.className ?? ''} readOnly value={this.props.selectedColor} />
                </div>
                <div className="relative ml-3 m-0.5">
                    <button type="button" className="w-10 h-10 rounded-full focus:outline-none focus:shadow-outline inline-flex p-2 shadow text-white" style={{ backgroundColor: this.props.selectedColor }} onClick={this.handleTogglePicker.bind(this)}>
                        <Picker className="w-6 h-6 text-gray-300" />
                    </button>
                    <div className={`origin-top-right absolute right-0 mt-2 w-64 rounded-md shadow-lg z-50 ${!this.state.pickerOpen ? 'hidden' : ''}`}>
                        <div className="rounded-md bg-white shadow-xs px-4 py-3">
                            <div >
                                {this.colorPalettes.map((colorPalette, index1) => {
                                    return (
                                        <div key={index1}>
                                            <div className="my-2 text-sm text-gray-600">{colorPalette.label}</div>
                                            <div className="flex flex-wrap -mx-2">
                                                {colorPalette.colors.map((color, index2) => {
                                                    return (
                                                        <div className="px-2" key={index2}>
                                                            {(this.props.selectedColor === color) ? (
                                                                <button type="button" name={this.props.name} id={`${this.props.id}-${index1}-${index2}`} className="w-8 h-8 inline-flex rounded-full cursor-pointer border-4 border-gray-200" style={{ background: color, boxShadow: '0 0 0 2px rgba(0, 0, 0, 0.2)' }} value={color} onClick={this.handleChange.bind(this)} />
                                                            ) : (
                                                                <button type="button" name={this.props.name} id={`${this.props.id}-${index1}-${index2}`} className="w-8 h-8 inline-flex rounded-full cursor-pointer border-4 border-gray-200 focus:outline-none focus:shadow-outline" style={{ background: color }} value={color} onClick={this.handleChange.bind(this)} />
                                                            )}
                                                        </div>
                                                    )
                                                })}
                                            </div>
                                        </div>
                                    )
                                })}
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
