import React from 'react'

export default class Label extends React.Component {

    render() {
        return(
            <label className="block text-gray-600 font-bold md:text-left mb-3 md:mb-0 pr-4" htmlFor={this.props.htmlFor}>
                {this.props.label ? this.props.label : this.props.children}
            </label>
        )
    }
}
