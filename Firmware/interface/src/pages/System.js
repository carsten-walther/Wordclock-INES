import React from 'react'

import Utility from '../utilities/Utility'

export default class System extends React.Component {
    render () {
        return (
            <div className="card">
                <h6 className="card-header">Software & Hardware</h6>
                <dl>
                    <div className="bg-gray-50">
                        <dt>Firmware Version</dt>
                        <dd>{this.props.info.version}</dd>
                    </div>
                    <div className="bg-white">
                        <dt>Chip ID</dt>
                        <dd>{this.props.info.chipId}</dd>
                    </div>
                    <div className="bg-gray-50">
                        <dt>SDK Version</dt>
                        <dd>{this.props.info.sdkVersion}</dd>
                    </div>
                    <div className="bg-white">
                        <dt>CPU Frequency</dt>
                        <dd>{this.props.info.cpuFreqMHz}</dd>
                    </div>
                    <div className="bg-gray-50">
                        <dt>Flash chip speed</dt>
                        <dd>{this.props.info.flashChipSpeed / 1000 / 1000} MHz</dd>
                    </div>
                    <div className="bg-white">
                        <dt>Flash chip size</dt>
                        <dd>{Utility.formatBytes(this.props.info.flashChipSize)}</dd>
                    </div>
                    <div className="bg-gray-50">
                        <dt>Free heap space</dt>
                        <dd>{Utility.formatBytes(this.props.info.freeHeap)}</dd>
                    </div>
                    <div className="bg-white">
                        <dt>MAC Address</dt>
                        <dd>{this.props.info.mac}</dd>
                    </div>
                </dl>
            </div>
        )
    }
}
