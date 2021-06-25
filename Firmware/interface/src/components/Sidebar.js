import React from 'react'

import Logo from './icons/Logo'
import NavItem from './NavItem'
import ChevronDown from './icons/ChevronDown'

export default class Sidebar extends React.Component {

    constructor (props) {
        super(props)
        this.state = {
            menuOpen: false
        }
    }

    toggleMenu() {
        this.setState({
            menuOpen: !this.state.menuOpen
        })
    }

    render() {
        return (
            <>
                <div className="block lg:hidden inset-0">
                    <h1 className="font-bold py-2 lg:pb-6 text-center text-gray-700 mx-auto mt-6 mb-6">
                        <Logo width={24} height={24} className="text-gray-700 inline -mt-1 mr-1" /> WordClock
                    </h1>
                    <div className="relative">
                        <button id="menu-toggle" className="flex w-full justify-start px-3 py-3 bg-white lg:bg-transparent border rounded border-gray-600 hover:border-gray-700 appearance-none focus:outline-none" onClick={this.toggleMenu.bind(this)}>
                            <span className="text-sm text-gray-600">Menu</span>
                        </button>
                        <div className="pointer-events-none absolute inset-y-0 right-0 flex items-center px-2 text-gray-700">
                            <ChevronDown className="h-5 w-5 float-right" />
                        </div>
                    </div>
                </div>
                <div className={`w-full sticky inset-0 ${!this.state.menuOpen ? 'hidden' : ''} lg:h-auto lg:block mt-0 my-2 lg:my-0 border border-gray-400 lg:border-transparent bg-white shadow rounded lg:shadow-none lg:bg-transparent z-20`} id="menu-content">
                    <Logo width={100} height={100} className="text-gray-700 ml-auto my-7 hidden lg:block"/>
                    <h1 className="text-right text-gray-700 text-xl font-bold pt-1.5 pb-4 hidden lg:block">WordClock</h1>
                    <hr className="mb-8 hidden lg:block"/>
                    <ul className="sidebar-list">
                        <li className="sidebar-list-item">
                            <NavItem to={'color-and-brightness'} title="Color & Brightness" />
                        </li>
                        <li className="sidebar-list-item">
                            <NavItem to={'mode-and-language'} title="Mode & Language" />
                        </li>
                        <li className="sidebar-list-header">
                            <span className="block pl-6 lg:pr-7 pb-1 md:pb-0 text-sm">Settings</span>
                        </li>
                        <li className="sidebar-list-item">
                            <NavItem to={'time-settings'} title="Time Settings" />
                        </li>
                        <li className="sidebar-list-item">
                            <NavItem to={'on-off-time'} title="On/Off Time" />
                        </li>
                        <li className="sidebar-list-item">
                            <NavItem to={'network'} title="Network" />
                        </li>
                        <li className="sidebar-list-item">
                            <NavItem to={'accessibility'} title="Accessibility" />
                        </li>
                        <li className="sidebar-list-item">
                            <NavItem to={'security'} title="Security" />
                        </li>
                        <li className="sidebar-list-item">
                            <NavItem to={'firmware'} title="Firmware" />
                        </li>
                        <li className="sidebar-list-header">
                            <span className="block pl-6 lg:pr-7 pb-1 md:pb-0 text-sm">Info</span>
                        </li>
                        <li className="sidebar-list-item">
                            <NavItem to={'system'} title="System" />
                        </li>
                        <li className="sidebar-list-item">
                            <NavItem to={'licences'} title="Licences" />
                        </li>
                    </ul>
                </div>
            </>
        )
    }
}
