/**
 * Color
 */
export default class Color {

    /**
     * rgb2hex
     *
     * @param r
     * @param g
     * @param b
     * @returns {string}
     */
    static rgb2hex (r, g, b) {
        return '#' + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1)
    }

    /**
     * hex2rgb
     *
     * @param hex
     * @returns {{r: number, b: number, g: number}|null}
     */
    static hex2rgb (hex) {
        let result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex)
        return result ? {
            r: parseInt(result[1], 16),
            g: parseInt(result[2], 16),
            b: parseInt(result[3], 16),
        } : null
    }
}
