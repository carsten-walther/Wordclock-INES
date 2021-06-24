import React from 'react'

export default class SectionHeader extends React.Component {

    render() {
        return(
            <h2 className="flex items-center font-sans font-bold break-normal text-gray-700 px-2 mt-12 lg:mt-2 text-md md:text-xl mb-6">
                {this.props.header}
            </h2>
        )
    }
}
