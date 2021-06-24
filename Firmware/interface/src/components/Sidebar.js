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
            <div className="w-full lg:w-1/5 px-6 text-xl text-gray-800 leading-normal">
                <div className="block lg:hidden inset-0">
                    <h1 className="font-bold py-2 lg:pb-6 text-center text-gray-700 mx-auto mb-6">
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
                    <h1 className="font-bold py-2 lg:pb-6 text-gray-700 hidden lg:block">
                        <Logo width={24} height={24} className="text-gray-700 inline -mt-1 -ml-2 mr-1" /> WordClock
                    </h1>
                    <ul className="list-reset py-2 md:py-0">
                        <li className="py-1 md:my-2 lg:border-l-2 border-transparent border-gray-700">
                            <NavItem to={'color-and-brightness'} title="Color & Brightness" />
                        </li>
                        <li className="py-1 md:my-2 lg:border-l-2 border-transparent border-gray-700">
                            <NavItem to={'mode-and-language'} title="Mode & Language" />
                        </li>
                        <li className="text-base font-bold py-2 lg:pt-4 lg:pb-2 text-gray-700">
                            <span className="pl-6 lg:pl-0 pb-1 md:pb-0 text-sm">Settings</span>
                        </li>
                        <li className="py-1 md:my-2 lg:border-l-2 border-transparent border-gray-700">
                            <NavItem to={'time-settings'} title="Time Settings" />
                        </li>
                        <li className="py-1 md:my-2 lg:border-l-2 border-transparent border-gray-700">
                            <NavItem to={'on-off-time'} title="On/Off Time" />
                        </li>
                        <li className="py-1 md:my-2 lg:border-l-2 border-transparent border-gray-700">
                            <NavItem to={'network'} title="Network" />
                        </li>
                        <li className="py-1 md:my-2 lg:border-l-2 border-transparent border-gray-700">
                            <NavItem to={'accessibility'} title="Accessibility" />
                        </li>
                        <li className="py-1 md:my-2 lg:border-l-2 border-transparent border-gray-700">
                            <NavItem to={'security'} title="Security" />
                        </li>
                        <li className="py-1 md:my-2 lg:border-l-2 border-transparent border-gray-700">
                            <NavItem to={'firmware'} title="Firmware" />
                        </li>
                        <li className="text-base font-bold py-2 lg:pt-4 lg:pb-2 text-gray-700">
                            <span className="pl-6 lg:pl-0 pb-1 md:pb-0 text-sm">Info</span>
                        </li>
                        <li className="py-1 md:my-2 lg:border-l-2 border-transparent border-gray-700">
                            <NavItem to={'system'} title="System" />
                        </li>
                        <li className="py-1 md:my-2 lg:border-l-2 border-transparent border-gray-700">
                            <NavItem to={'licences'} title="Licences" />
                        </li>
                    </ul>
                </div>
            </div>
        )
    }
}
