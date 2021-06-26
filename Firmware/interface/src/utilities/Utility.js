/**
 * Utility
 */
export default class Utility {

    /**
     * formatTime
     *
     * @param date
     * @returns {string}
     */
    static formatTime(date) {
        let d = new Date(date)
        let hours = d.getHours() < 10 ? '0' + d.getHours() : d.getHours()
        let minutes = d.getMinutes() < 10 ? '0' + d.getMinutes() : d.getMinutes()
        return [hours, minutes].join(':')
    }

    /**
     * formatDate
     *
     * @param date
     * @param full
     * @returns {string}
     */
    static formatDate(date, full) {
        const monthNames = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"]
        let d = new Date(date)
        if (full) {
            return d.getUTCDate() + '. ' + monthNames[d.getMonth()] + ' ' + d.getUTCFullYear()
        } else {
            return [d.getUTCDate(), d.getUTCMonth(), d.getUTCFullYear()].join('.')
        }
    }

    /**
     * combineHourAndMinute
     *
     * @param hour
     * @param minute
     * @returns {string}
     */
    static combineHourAndMinute(hour, minute) {
        let h = ''
        if (hour < 10) {
            h = '0' + hour
        } else {
            h = hour
        }
        let m = ''
        if (minute < 10) {
            m = '0' + minute
        } else {
            m = minute
        }
        return h + ':' + m
    }

    /**
     * separateHourAndMinute
     *
     * @param timeString
     * @returns {{h, m}}
     */
    static separateHourAndMinute(timeString) {
        let timeArray = timeString.split(':').map(Number)
        return {
            h: timeArray[0],
            m: timeArray[1]
        }
    }

    /**
     * formatBytes
     *
     * @param bytes
     * @param decimals
     * @returns {string}
     */
    static formatBytes (bytes, decimals) {
        if (bytes === 0) return '0 B'
        let k = 1024
        let sizes = ['B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB']
        let i = Math.floor(Math.log(bytes) / Math.log(k))
        return parseFloat((bytes / Math.pow(k, i)).toFixed(decimals)) + ' ' + sizes[i]
    }

    /**
     * getFileBasename
     *
     * @param path
     * @returns {*}
     */
    static getFileBasename (path) {
        return path.split('/').reverse()[0]
    }

    /**
     * getFileExtension
     *
     * @param path
     * @returns {*}
     */
    static getFileExtension (path) {
        return this.getFileBasename(path).split('.').reverse()[0]
    }
}
