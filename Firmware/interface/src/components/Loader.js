import React from 'react'

import Backdrop from './Backdrop'

export default class Loader extends React.Component {

    render() {
        if (this.props.isBusy === false) {
            return null
        } else {
            return (
                <Backdrop>
                    <div className="flex h-screen">
                        <div className="m-auto">
                            <div className="flex relative justify-around">
                                <span className="flex absolute h-3 w-3 -top-4 mx-auto">
                                    <span className="animate-ping absolute inline-flex h-full w-full rounded-full bg-red-400 opacity-75" />
                                    <span className="relative inline-flex rounded-full h-3 w-3 bg-red-500" />
                                </span>
                                <div className="inline-flex">
                                    <span className="inline-flex items-center leading-6 font-medium">
                                        Loading
                                    </span>
                                </div>
                            </div>
                        </div>
                    </div>
                </Backdrop>
            )
        }
    }
}
