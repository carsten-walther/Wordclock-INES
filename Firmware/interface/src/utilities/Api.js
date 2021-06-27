/**
 * Api
 */
export default class Api {

    static baseUrl = '/'

    // Resource: /api/wifi
    //
    //

    /**
     * wifiGet
     *
     * @returns {Promise<*>}
     */
    static async wifiGet() {
        return await fetch(this.baseUrl + "api/wifi/get")
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * wifiSet
     *
     * @param data
     * @returns {Promise<*>}
     */
    static async wifiSet (data) {
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
     * wifiSetStatic
     *
     * @param data
     * @returns {Promise<*>}
     */
    static async wifiSetStatic (data) {
        let formData = new FormData()

        Object.keys(data).map(key => formData.append(key, data[key]))

        let init = {
            method: 'POST',
            body: formData
        }

        return await fetch(this.baseUrl + "api/wifi/setStatic", init)
          .then(response => response.json())
          .then(result => result, error => error)
    }

    /**
     * wifiScan
     *
     * @returns {Promise<*>}
     */
    static async wifiScan () {
        return await fetch(this.baseUrl + "api/wifi/scan")
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * wifiForget
     *
     * @returns {Promise<*>}
     */
    static async wifiForget () {
        let init = {
            method: 'POST'
        }

        return await fetch(this.baseUrl + "api/wifi/forget", init)
            .then(response => response.json())
            .then(result => result, error => error)
    }

    // Resource: /api/files
    //
    //

    /**
     * filesGet
     *
     * @returns {Promise<*>}
     */
    static async filesGet() {
        return await fetch(this.baseUrl + "api/files/get")
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * filesRemove
     *
     * @param filename
     * @returns {Promise<*>}
     */
    static async filesRemove (filename) {
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
    static async filesUpload (data, overwrite) {
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
    //
    //

    /**
     * systemRestart
     *
     * @returns {Promise<*>}
     */
    static async systemRestart () {
        let init = {
            method: 'POST'
        }

        return await fetch(this.baseUrl + "api/system/restart", init)
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * systemUpdate
     *
     * @returns {Promise<*>}
     */
    static async systemUpdate () {
        let init = {
            method: 'POST'
        }

        return await fetch(this.baseUrl + "api/system/update", init)
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * systemUpdateStatus
     *
     * @returns {Promise<*>}
     */
    static async systemUpdateStatus () {
        return await fetch(this.baseUrl + "api/system/update-status")
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * systemInfo
     *
     * @returns {Promise<*>}
     */
    static async systemInfo () {
        return await fetch(this.baseUrl + "api/system/info")
            .then(response => response.json())
            .then(result => result, error => error)
    }

    // Resource: /api/config
    //
    //

    /**
     * configGet
     *
     * @returns {Promise<*>}
     */
    static async configGet() {
        return await fetch(this.baseUrl + "api/config/get")
            .then(response => response.json())
            .then(result => result, error => error)
    }

    /**
     * configSet
     *
     * @param data
     * @returns {Promise<*>}
     */
    static async configSet (data) {
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
}
