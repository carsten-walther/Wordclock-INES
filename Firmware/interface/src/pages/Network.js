import React from 'react'

import SectionHeader from '../components/SectionHeader'
import ChevronDown from '../components/icons/ChevronDown'
import Divider from '../components/Divider'
import Label from '../components/Form/Label'

export default class Network extends React.Component {
    render () {
        return (
            <>
                <SectionHeader header="Network" />
                <div className="p-8 mt-6 lg:mt-0 rounded shadow bg-white">
                    <form onSubmit={this.props.onSubmit.bind(this)}>
                        <div className="md:flex mb-6">
                            <div className="md:w-1/3">
                                <Label htmlFor="ssid">SSID</Label>
                            </div>
                            <div className="md:w-2/3">
                                <div className="relative">
                                    <select name="ssid" id="ssid" value={this.props.data.ssid} className="appearance-none block leading-none w-full text-gray-700 border border-gray-300 rounded py-3 px-4" onChange={this.props.onChange.bind(this)}>
                                        <option value="">SSID</option>
                                        {this.props.networks && this.props.networks.map((network, index) => {
                                            return (
                                                <option value={network.ssid} key={index}>
                                                    {(network.encryptionType > 0) ? '🔒 ' : '🔓 '} {network.ssid} {(network.encryptionType > 0) ? '(closed)' : '(open)'}
                                                </option>
                                            )
                                        })}
                                    </select>
                                    <div className="pointer-events-none absolute inset-y-0 right-0 flex items-center px-2 text-gray-700">
                                        <ChevronDown className="h-5 w-5 float-right" />
                                    </div>
                                </div>
                                <p className="m-0 py-2 text-sm text-gray-500">Please select your preferred WLAN.</p>
                            </div>
                        </div>
                        <div className="md:flex mb-6">
                            <div className="md:w-1/3">
                                <Label htmlFor="pass">Password</Label>
                            </div>
                            <div className="md:w-2/3">
                                <input type="password" name="pass" id="pass" className="appearance-none block leading-none w-full text-gray-700 border border-gray-300 rounded py-3 px-4" value={this.props.data.pass} onChange={this.props.onChange.bind(this)} />
                                <p className="m-0 py-2 text-sm text-gray-500">Please enter your WLAN password.</p>
                            </div>
                        </div>
                        <div className="md:flex md:items-center mb-6">
                            <div className="md:w-1/3" />
                            <div className="md:w-2/3">
                                <div className="flex">
                                    <div className="w-1/2">
                                        <button type="button" className="float-left underline text-sm cursor-pointer" onClick={this.props.onNetworkScan.bind(this)}>Scan for networks!</button>
                                    </div>
                                    <div className="w-1/2">
                                        <button type="button" className="float-right underline text-sm cursor-pointer" onClick={this.props.onNetworkReset.bind(this)}>Reset network settings!</button>
                                    </div>
                                </div>
                            </div>
                        </div>
                        <Divider className="bg-gray-300 my-6" />
                        <div className="md:flex mb-6">
                            <div className="md:w-1/3">
                                <Label htmlFor="hostname">Hostname</Label>
                            </div>
                            <div className="md:w-2/3">
                                <input type="text" name="hostname" id="hostname" className="appearance-none block leading-none w-full text-gray-700 border border-gray-300 rounded py-3 px-4" value={this.props.data.hostname} onChange={this.props.onChange.bind(this)} />
                                <p className="m-0 py-2 text-sm text-gray-500">Enter the hostname to connect to wordclock. (.local will be added automatically)</p>
                            </div>
                        </div>
                        <Divider className="bg-gray-300 my-6" />
                        <div className="md:flex mb-6">
                            <div className="md:w-1/3">
                                <Label htmlFor="useAuth">Use Authentication</Label>
                            </div>
                            <div className="md:w-2/3">
                                <div>
                                    <label className="inline-flex items-center">
                                        <input type="checkbox" name="useAuth" id="useAuth" className="form-checkbox text-indigo-600" checked={this.props.data.useAuth} onChange={this.props.onChange.bind(this)} />
                                        <span className="ml-2">Use basic authentication for access.</span>
                                    </label>
                                </div>
                                <p className="m-0 py-2 text-sm text-gray-500">Are you using daylight saving time?</p>
                            </div>
                        </div>
                        <div className="md:flex mb-6">
                            <div className="md:w-1/3">
                                <Label htmlFor="authUsername">Username</Label>
                            </div>
                            <div className="md:w-2/3">
                                <input type="text" name="authUsername" id="authUsername" className="appearance-none block leading-none w-full text-gray-700 border border-gray-300 rounded py-3 px-4" value={this.props.data.authUsername} onChange={this.props.onChange.bind(this)} />
                                <p className="m-0 py-2 text-sm text-gray-500">Choose an username for authentication.</p>
                            </div>
                        </div>
                        <div className="md:flex mb-6">
                            <div className="md:w-1/3">
                                <Label htmlFor="authPassword">Password</Label>
                            </div>
                            <div className="md:w-2/3">
                                <input type="password" name="authPassword" id="authPassword" className="appearance-none block leading-none w-full text-gray-700 border border-gray-300 rounded py-3 px-4" value={this.props.data.authPassword} onChange={this.props.onChange.bind(this)} />
                                <p className="m-0 py-2 text-sm text-gray-500">Enter the password for authentication.</p>
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
