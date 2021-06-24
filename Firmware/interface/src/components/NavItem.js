import React from 'react'
import { Link, scrollSpy } from 'react-scroll'

export default class NavItem extends React.Component {

    componentDidMount() {
        scrollSpy.update()
    }

    render() {
        return(
            <>
                <Link to={this.props.to} spy hashSpy smooth offset={-10} className="block pl-6 align-middle text-gray-700 no-underline hover:text-red-600" activeClass="text-red-600">
                    <span className="pb-1 md:pb-0 text-sm">{this.props.title}</span>
                </Link>
            </>
        )
    }
}
