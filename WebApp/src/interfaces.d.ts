/**
 * interface Mode
 */
interface Mode {
  label: string,
  value: number
}

/**
 * interface Timezone
 */
interface Timezone {
  label: string,
  value: number
}

/**
 * interface Language
 */
interface Language {
  label: string,
  value: number
}

/**
 * interface Favorite
 */
interface Favorite {
  title: string,
  foregroundColor: Color,
  backgroundColor: Color,
  brightness: number,
  locked?: boolean
}

/**
 * interface Color
 */
interface Color {
  red: number,
  green: number,
  blue: number
}

/**
 * interface Settings
 */
interface Settings {
  // clock
  foregroundColor?: Color,
  backgroundColor?: Color,
  // settings
  brightness?: number,
  timeZone?: number,
  daylightSavingsTime?: boolean,
  sleepHour?: number,
  sleepMinute?: number,
  wakeupHour?: number,
  wakeupMinute?: number,
  language?: number,
  clockMode?: number,
  // stuff
  version?: string,
  eeprom?: string,
  time?: string
}
