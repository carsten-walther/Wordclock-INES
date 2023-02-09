import React from 'react'
import { Link, scrollSpy } from 'react-scroll'

export default class NavItem extends React.Component {

    componentDidMount () {
        scrollSpy.update()
    }

    render () {
        return (
            <>
                <Link to={this.props.to} spy hashSpy smooth className="sidebar-navitem text-gray-700 dark:text-gray-50" activeClass="text-red-600 dark:text-red-500">
                    {this.props.title}
                </Link>
            </>
        )
    }
}
