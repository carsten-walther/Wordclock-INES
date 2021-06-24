import React from 'react'

import Api from '../utilities/Api'
import SectionHeader from '../components/SectionHeader'
import Loader from '../components/Loader'
import Label from '../components/Form/Label'

export default class Firmware extends React.Component {

    constructor (props) {
        super(props)
        this.state = {
            isLoading: false,
            file: null
        }
    }

    async uploadAndUpdate () {
        await Api.uploadFile(this.state.file, true).then(async (result) => {
            this.setState({
                isLoading: false
            })
            await Api.update()
            await Api.updateStatus()
            this.setState({
                data: { ...this.state, file: null }
            })
        })
    }

    handleChange (event) {
        this.setState({
            data: { ...this.state, file: event.target.files[0] }
        })
    }

    async handleSubmit (event) {
        event.preventDefault()
        this.setState({
            isLoading: true
        })
        await this.uploadAndUpdate()
    }

    render () {
        return (
            <>
                <SectionHeader header="Firmware" />
                <div className="p-8 mt-6 lg:mt-0 rounded shadow bg-white">
                    <form onSubmit={this.handleSubmit.bind(this)}>
                        <div className="md:flex mb-6">
                            <div className="md:w-1/3">
                                <Label htmlFor="file">Firmware File</Label>
                            </div>
                            <div className="md:w-2/3">
                                <input type="file" name="file" id="file" className="appearance-none block leading-none w-full text-gray-700 border border-gray-300 rounded py-3 px-4" placeholder={(this.state.file ? this.state.file.name : null) || 'Choose file'} onChange={this.handleChange.bind(this)} />
                                <p className="py-2 text-sm text-gray-600">Please select the firmware file (BIN up to 500kB).</p>
                            </div>
                        </div>
                        <div className="md:flex md:items-center">
                            <div className="md:w-1/3" />
                            <div className="md:w-2/3">
                                <button type="submit" className="shadow bg-red-600 hover:bg-red-500 focus:shadow-outline focus:outline-none text-white font-bold py-2 px-4 rounded">Upload and Update Firmware</button>
                            </div>
                        </div>
                    </form>
                </div>
                <Loader isBusy={this.state.isLoading}/>
            </>
        )
    }
}
