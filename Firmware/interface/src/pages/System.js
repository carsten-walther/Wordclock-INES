import React from 'react'
import Utility from '../utilities/Utility'

export default class System extends React.Component {
    render () {
        return (
            <div id="system" className="card">
                <h6 className="card-header">
                    Software & Hardware
                </h6>
                <table className="table-auto w-full">
                    <tbody>
                    <tr className="bg-gray-100 dark:bg-gray-500">
                        <th>Firmware Version</th>
                        <td>{this.props.info.version}</td>
                    </tr>
                    <tr>
                        <th>MAC Address</th>
                        <td>{this.props.info.mac}</td>
                    </tr>
                    <tr className="bg-gray-100 dark:bg-gray-500">
                        <th>SDK Version</th>
                        <td>{this.props.info.sdkVersion}</td>
                    </tr>
                    <tr>
                        <th>Free Heap</th>
                        <td>{Utility.formatBytes(this.props.info.freeHeap)}</td>
                    </tr>
                    <tr className="bg-gray-100 dark:bg-gray-500">
                        <th>Boot Mode</th>
                        <td>{this.props.info.bootMode}</td>
                    </tr>
                    <tr>
                        <th>Boot Version</th>
                        <td>{this.props.info.bootVersion}</td>
                    </tr>
                    <tr className="bg-gray-100 dark:bg-gray-500">
                        <th>Chip Id</th>
                        <td>{this.props.info.chipId}</td>
                    </tr>
                    <tr>
                        <th>Flash Chip Id</th>
                        <td>{this.props.info.flashChipId}</td>
                    </tr>
                    <tr className="bg-gray-100 dark:bg-gray-500">
                        <th>Flash Chip Real Size</th>
                        <td>{Utility.formatBytes(this.props.info.flashChipRealSize)}</td>
                    </tr>
                    <tr>
                        <th>Flash Chip Size</th>
                        <td>{Utility.formatBytes(this.props.info.flashChipSize)}</td>
                    </tr>
                    <tr className="bg-gray-100 dark:bg-gray-500">
                        <th>Flash Chip Size By Chip Id</th>
                        <td>{Utility.formatBytes(this.props.info.flashChipSizeByChipId)}</td>
                    </tr>
                    <tr>
                        <th>Free Sketch Space</th>
                        <td>{Utility.formatBytes(this.props.info.freeSketchSpace)}</td>
                    </tr>
                    <tr className="bg-gray-100 dark:bg-gray-500">
                        <th>OTA Enabled</th>
                        <td>{this.props.info.otaEnabled ? 'true' : 'false'}</td>
                    </tr>
                    <tr>
                        <th>Updater Enabled</th>
                        <td>{this.props.info.updaterEnabled ? 'true' : 'false'}</td>
                    </tr>
                    <tr className="bg-gray-100 dark:bg-gray-500">
                        <th>Restart WordClock</th>
                        <td>
                            <button type="button" className="form-btn-red" onClick={this.props.onSystemRestart.bind(this)}>Restart</button>
                        </td>
                    </tr>
                    </tbody>
                </table>
            </div>
        )
    }
}
