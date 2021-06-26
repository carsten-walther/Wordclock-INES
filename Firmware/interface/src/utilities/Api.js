/**
 * Api
 */
export default class Api {

    static baseUrl = '/'
    //static baseUrl = 'http://wordclock-two.local/'

    // Resource: /api/wifi

    /**
     * getWifi
     *
     * @returns {Promise<*>}
     */
    static async getWifi() {
        return await fetch(this.baseUrl + "api/wifi/get")
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * setWifi
     *
     * @param data
     * @returns {Promise<*>}
     */
    static async setWifi (data) {
        let formData = new FormData()

        Object.keys(data).map(key => formData.append(key, data[key]))

        let init = {
            method: 'POST',
            body: formData
        }

        return await fetch(this.baseUrl + "api/wifi/set", init)
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * scanWifi
     *
     * @returns {Promise<*>}
     */
    static async scanWifi () {
        return await fetch(this.baseUrl + "api/wifi/scan")
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * resetWifi
     *
     * @returns {Promise<*>}
     */
    static async resetWifi () {
        let init = {
            method: 'POST'
        }

        return await fetch(this.baseUrl + "api/wifi/reset", init)
            .then(response => response.json())
            .then(result => result, error => error)
    }

    // Resource: /api/files

    /**
     * getFiles
     *
     * @returns {Promise<*>}
     */
    static async getFiles() {
        return await fetch(this.baseUrl + "api/files/get")
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * removeFile
     *
     * @param filename
     * @returns {Promise<*>}
     */
    static async removeFile (filename) {
        let formData = new FormData()

        formData.append('filename', filename)

        let init = {
            method: 'POST',
            body: formData
        }

        return await fetch(this.baseUrl + "api/files/remove", init)
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * upload
     *
     * @param data
     * @param overwrite
     * @returns {Promise<*>}
     */
    static async uploadFile (data, overwrite) {
        let formData = new FormData()

        formData.append('file', data)
        formData.append('overwrite', overwrite)

        let init = {
            method: 'POST',
            body: formData
        }

        return await fetch(this.baseUrl + "api/files/upload", init)
            .then(response => response.json())
            .then(result => result, error => error)
    }

    // Resource: /api

    /**
     * restart
     *
     * @returns {Promise<*>}
     */
    static async restart () {
        let init = {
            method: 'POST'
        }

        return await fetch(this.baseUrl + "api/restart", init)
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * update
     *
     * @returns {Promise<*>}
     */
    static async update () {
        let init = {
            method: 'POST'
        }

        return await fetch(this.baseUrl + "api/update", init)
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * updateStatus
     *
     * @returns {Promise<*>}
     */
    static async updateStatus () {
        return await fetch(this.baseUrl + "api/update-status")
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * loadTimers
     *
     * @returns {Promise<*>}
     */
    static async loadTimers () {
        return await fetch(this.baseUrl + "api/load-timers")
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * info
     *
     * @returns {Promise<*>}
     */
    static async info () {
        return await fetch(this.baseUrl + "api/info")
            .then(response => response.json())
            .then(result => result, error => error)
    }

    // Resource: /api/config

    /**
     * getConfig
     *
     * @returns {Promise<*>}
     */
    static async getConfig() {
        return await fetch(this.baseUrl + "api/config/get")
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * setConfig
     *
     * @param data
     * @returns {Promise<*>}
     */
    static async setConfig (data) {
        let formData = new FormData()

        Object.keys(data).map(key => formData.append(key, data[key]))

        let init = {
            method: 'POST',
            body: formData
        }

        return await fetch(this.baseUrl + "api/config/set", init)
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * getData
     *
     * @param filename
     * @returns {Promise<*>}
     */
    static async getData (filename) {
        return await fetch(this.baseUrl + "data/" + filename)
            .then(response => {
                if (response.status === 200) return response.json()
            })
            .then(result => result, error => error)
    }
}
