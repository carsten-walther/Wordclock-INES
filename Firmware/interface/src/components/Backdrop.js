import React from 'react'
import ReactDOM from 'react-dom'

export default class Backdrop extends React.Component {

    constructor (props) {
        super(props)
        this.el = document.createElement('div')
        this.el.classList.add('fixed')
        this.el.classList.add('w-full')
        this.el.classList.add('top-0')
        this.el.classList.add('left-0')
        this.el.classList.add('right-0')
        this.el.classList.add('bottom-0')
        this.el.classList.add('backdrop-filter')
        this.el.classList.add('backdrop-blur')
        this.el.classList.add('z-50')
    }

    componentDidMount () {
        document.body.appendChild(this.el)
    }

    componentWillUnmount () {
        document.body.removeChild(this.el)
    }

    render () {
        return ReactDOM.createPortal(
            this.props.children,
            this.el
        )
    }
}
