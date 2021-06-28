import React from 'react'

import Api from '../utilities/Api'
import Loader from '../components/Loader'

export default class Firmware extends React.Component {

    constructor (props) {
        super(props)
        this.state = {
            isLoading: false,
            file: null,
        }
    }

    async uploadAndUpdate () {
        await Api.filesUpload(this.state.file, true).then(async (result) => {
            this.setState({
                isLoading: false,
            })
            await Api.systemUpdate()
            await Api.systemUpdateStatus()
            this.setState({
                data: { ...this.state, file: null },
            })
        })
    }

    handleChange (event) {
        this.setState({
            data: { ...this.state, file: event.target.files[0] },
        })
    }

    async handleSubmit (event) {
        event.preventDefault()
        this.setState({
            isLoading: true,
        })
        await this.uploadAndUpdate()
    }

    render () {
        return (
            <>

                <div id="firmware" className="card">
                    <h2 className="card-header">
                        Firmware
                    </h2>
                    <div className="card-body">
                        <div className="md:flex mb-6">
                            <div className="md:w-1/3">
                                <label className="form-label" htmlFor="file">Firmware File</label>
                            </div>
                            <div className="md:w-2/3">
                                <input type="file" name="file" id="file" className="form-input" placeholder={(this.state.file ? this.state.file.name : null) || 'Choose file'} onChange={this.handleChange.bind(this)}/>
                                <p className="help-text">Select the firmware file (BIN up to 500kB).</p>
                            </div>
                        </div>
                        <div className="md:flex md:items-center">
                            <div className="md:w-1/3"/>
                            <div className="md:w-2/3">
                                <button type="submit" className="w-full form-btn-red">Upload and Update Firmware</button>
                            </div>
                        </div>
                    </div>
                </div>
                <Loader isBusy={this.state.isLoading}/>
            </>
        )
    }
}
