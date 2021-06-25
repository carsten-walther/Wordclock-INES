import React from 'react'

import ChevronDown from '../components/icons/ChevronDown'

export default class TimeSettings extends React.Component {
    render () {
        return (
            <div id="time-settings" className="card">
                <h2 className="card-header">
                    Time Settings
                    <button type="button" className="float-right form-btn-red text-sm py-1 px-2" onClick={this.props.onSubmit.bind(this)}>Save</button>
                </h2>
                <div className="card-body">
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="ntpServer">Network Time Server</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="text" name="ntpServer" id="ntpServer" className="form-input" placeholder="0.pool.ntp.org" value={this.props.data.ntpServer} onChange={this.props.onChange.bind(this)} />
                            <p className="help-text">Please select your preferred time server.</p>
                        </div>
                    </div>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="ntpSyncInterval">Time Update Interval</label>
                        </div>
                        <div className="md:w-2/3">
                            <input type="number" name="ntpSyncInterval" id="ntpSyncInterval" className="form-input" placeholder="0.pool.ntp.org" value={this.props.data.ntpSyncInterval} onChange={this.props.onChange.bind(this)} />
                            <p className="help-text">Enter the time in seconds, when a time sync should be done.</p>
                        </div>
                    </div>
                    <div className="md:flex mb-6">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="timezone">Timezone</label>
                        </div>
                        <div className="md:w-2/3">
                            <div className="relative">
                                <select name="timezone" id="timezone" value={this.props.data.timezone} className="form-input pr-7" onChange={this.props.onChange.bind(this)}>
                                    <option key={0} value="0">(GMT-12:00) International Date Line West</option>
                                    <option key={1} value="1">(GMT-11:00) Mvalueway Island, Samoa</option>
                                    <option key={2} value="2">(GMT-10:00) Hawaii</option>
                                    <option key={3} value="3">(GMT-09:00) Alaska</option>
                                    <option key={4} value="4">(GMT-08:00) Pacific Time (US & Canada)</option>
                                    <option key={5} value="5">(GMT-08:00) Tijuana, Baja California</option>
                                    <option key={6} value="6">(GMT-07:00) Arizona</option>
                                    <option key={7} value="7">(GMT-07:00) Chihuahua, La Paz, Mazatlan</option>
                                    <option key={8} value="8">(GMT-07:00) Mountain Time (US & Canada)</option>
                                    <option key={9} value="9">(GMT-06:00) Central America</option>
                                    <option key={10} value="10">(GMT-06:00) Central Time (US & Canada)</option>
                                    <option key={11} value="11">(GMT-06:00) Guadalajara, Mexico City, Monterrey</option>
                                    <option key={12} value="12">(GMT-06:00) Saskatchewan</option>
                                    <option key={13} value="13">(GMT-05:00) Bogota, Lima, Quito, Rio Branco</option>
                                    <option key={14} value="14">(GMT-05:00) Eastern Time (US & Canada)</option>
                                    <option key={15} value="15">(GMT-05:00) Indiana (East)</option>
                                    <option key={16} value="16">(GMT-04:00) Atlantic Time (Canada)</option>
                                    <option key={17} value="17">(GMT-04:00) Caracas, La Paz</option>
                                    <option key={18} value="18">(GMT-04:00) Manaus</option>
                                    <option key={19} value="19">(GMT-04:00) Santiago</option>
                                    <option key={20} value="20">(GMT-03:30) Newfoundland</option>
                                    <option key={21} value="21">(GMT-03:00) Brasilia</option>
                                    <option key={22} value="22">(GMT-03:00) Buenos Aires, Georgetown</option>
                                    <option key={23} value="23">(GMT-03:00) Greenland</option>
                                    <option key={24} value="24">(GMT-03:00) Montevvalueeo</option>
                                    <option key={25} value="25">(GMT-02:00) Mvalue-Atlantic</option>
                                    <option key={26} value="26">(GMT-01:00) Cape Verde Is.</option>
                                    <option key={27} value="27">(GMT-01:00) Azores</option>
                                    <option key={28} value="28">(GMT+00:00) Casablanca, Monrovia, Reykjavik</option>
                                    <option key={29} value="29">(GMT+00:00) Greenwich Mean Time : Dublin, Edinburgh, Lisbon, London</option>
                                    <option key={30} value="30">(GMT+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna</option>
                                    <option key={31} value="31">(GMT+01:00) Belgrade, Bratislava, Budapest, Ljubljana, Prague</option>
                                    <option key={32} value="32">(GMT+01:00) Brussels, Copenhagen, Madrvalue, Paris</option>
                                    <option key={33} value="33">(GMT+01:00) Sarajevo, Skopje, Warsaw, Zagreb</option>
                                    <option key={34} value="34">(GMT+01:00) West Central Africa</option>
                                    <option key={35} value="35">(GMT+02:00) Amman</option>
                                    <option key={36} value="36">(GMT+02:00) Athens, Bucharest, Istanbul</option>
                                    <option key={37} value="37">(GMT+02:00) Beirut</option>
                                    <option key={38} value="38">(GMT+02:00) Cairo</option>
                                    <option key={39} value="39">(GMT+02:00) Harare, Pretoria</option>
                                    <option key={40} value="40">(GMT+02:00) Helsinki, Kyiv, Riga, Sofia, Tallinn, Vilnius</option>
                                    <option key={41} value="41">(GMT+02:00) Jerusalem</option>
                                    <option key={42} value="42">(GMT+02:00) Minsk</option>
                                    <option key={43} value="43">(GMT+02:00) Windhoek</option>
                                    <option key={44} value="44">(GMT+03:00) Kuwait, Riyadh, Baghdad</option>
                                    <option key={45} value="45">(GMT+03:00) Moscow, St. Petersburg, Volgograd</option>
                                    <option key={46} value="46">(GMT+03:00) Nairobi</option>
                                    <option key={47} value="47">(GMT+03:00) Tbilisi</option>
                                    <option key={48} value="48">(GMT+03:30) Tehran</option>
                                    <option key={49} value="49">(GMT+04:00) Abu Dhabi, Muscat</option>
                                    <option key={50} value="50">(GMT+04:00) Baku</option>
                                    <option key={51} value="51">(GMT+04:00) Yerevan</option>
                                    <option key={52} value="52">(GMT+04:30) Kabul</option>
                                    <option key={53} value="53">(GMT+05:00) Yekaterinburg</option>
                                    <option key={54} value="54">(GMT+05:00) Islamabad, Karachi, Tashkent</option>
                                    <option key={55} value="55">(GMT+05:30) Sri Jayawardenapura</option>
                                    <option key={56} value="56">(GMT+05:30) Chennai, Kolkata, Mumbai, New Delhi</option>
                                    <option key={57} value="57">(GMT+05:45) Kathmandu</option>
                                    <option key={58} value="58">(GMT+06:00) Almaty, Novosibirsk</option>
                                    <option key={59} value="59">(GMT+06:00) Astana, Dhaka</option>
                                    <option key={60} value="60">(GMT+06:30) Yangon (Rangoon)</option>
                                    <option key={61} value="61">(GMT+07:00) Bangkok, Hanoi, Jakarta</option>
                                    <option key={62} value="62">(GMT+07:00) Krasnoyarsk</option>
                                    <option key={63} value="63">(GMT+08:00) Beijing, Chongqing, Hong Kong, Urumqi</option>
                                    <option key={64} value="64">(GMT+08:00) Kuala Lumpur, Singapore</option>
                                    <option key={65} value="65">(GMT+08:00) Irkutsk, Ulaan Bataar</option>
                                    <option key={66} value="66">(GMT+08:00) Perth</option>
                                    <option key={67} value="67">(GMT+08:00) Taipei</option>
                                    <option key={68} value="68">(GMT+09:00) Osaka, Sapporo, Tokyo</option>
                                    <option key={69} value="69">(GMT+09:00) Seoul</option>
                                    <option key={70} value="70">(GMT+09:00) Yakutsk</option>
                                    <option key={71} value="71">(GMT+09:30) Adelavaluee</option>
                                    <option key={72} value="72">(GMT+09:30) Darwin</option>
                                    <option key={73} value="73">(GMT+10:00) Brisbane</option>
                                    <option key={74} value="74">(GMT+10:00) Canberra, Melbourne, Sydney</option>
                                    <option key={75} value="75">(GMT+10:00) Hobart</option>
                                    <option key={76} value="76">(GMT+10:00) Guam, Port Moresby</option>
                                    <option key={77} value="77">(GMT+10:00) Vladivostok</option>
                                    <option key={78} value="78">(GMT+11:00) Magadan, Solomon Is., New Caledonia</option>
                                    <option key={79} value="79">(GMT+12:00) Auckland, Wellington</option>
                                    <option key={80} value="80">(GMT+12:00) Fiji, Kamchatka, Marshall Is.</option>
                                    <option key={81} value="81">(GMT+13:00) Nuku'alofa</option>
                                </select>
                                <div className="form-input-chevron">
                                    <ChevronDown className="h-5 w-5 float-right" />
                                </div>
                            </div>
                            <p className="help-text">Please select your local timezone.</p>
                        </div>
                    </div>
                    <div className="md:flex">
                        <div className="md:w-1/3">
                            <label className="form-label" htmlFor="daylightSavingTime">Daylight Saving Time</label>
                        </div>
                        <div className="md:w-2/3">
                            <div>
                                <label className="inline-flex items-center">
                                    <input type="checkbox" name="daylightSavingTime" id="daylightSavingTime" className="form-checkbox text-indigo-600" checked={this.props.data.daylightSavingTime} onChange={this.props.onChange.bind(this)} />
                                    <span className="ml-2">Daylight Saving Time</span>
                                </label>
                            </div>
                            <p className="help-text">Are you using daylight saving time?</p>
                        </div>
                    </div>
                </div>
            </div>
        )
    }
}
