import React from 'react'

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
                        <th>Restart WordClock</th>
                        <td>
                            <button type="button" className="form-btn-red " onClick={this.props.onSystemRestart.bind(this)}>Restart</button>
                        </td>
                    </tr>
                    </tbody>
                </table>
            </div>
        )
    }
}
