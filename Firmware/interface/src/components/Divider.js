import React from 'react'

export default class Divider extends React.Component {

    render() {
        return(
            <hr className={this.props.className ?? 'bg-gray-300 my-12'} />
        )
    }
}
