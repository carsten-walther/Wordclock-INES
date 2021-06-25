import React from 'react'

import Utility from '../utilities/Utility'

export default class System extends React.Component {
    render () {
        return (
            <div id="system" className="card">
                <h6 className="card-header">Software & Hardware</h6>
                <table className="table-auto w-full">
                <tbody>
                    <tr className="bg-gray-100">
                        <th>Firmware Version</th>
                        <td>{this.props.info.version}</td>
                    </tr>
                    <tr>
                        <th>Chip ID</th>
                        <td>{this.props.info.chipId}</td>
                    </tr>
                    <tr className="bg-gray-100">
                        <th>SDK Version</th>
                        <td>{this.props.info.sdkVersion}</td>
                    </tr>
                    <tr>
                        <th>CPU Frequency</th>
                        <td>{this.props.info.cpuFreqMHz}</td>
                    </tr>
                    <tr className="bg-gray-100">
                        <th>Flash Chip Speed</th>
                        <td>{this.props.info.flashChipSpeed / 1000 / 1000} MHz</td>
                    </tr>
                    <tr>
                        <th>Flash Chip Size</th>
                        <td>{Utility.formatBytes(this.props.info.flashChipSize)}</td>
                    </tr>
                    <tr className="bg-gray-100">
                        <th>Free Heap Space</th>
                        <td>{Utility.formatBytes(this.props.info.freeHeap)}</td>
                    </tr>
                    <tr>
                        <th>MAC Address</th>
                        <td>{this.props.info.mac}</td>
                    </tr>
                </tbody>
                </table>
            </div>
        )
    }
}
