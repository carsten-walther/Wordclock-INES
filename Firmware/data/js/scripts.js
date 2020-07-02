let rgbToHex = function (rgb) {
  let hex = Number(rgb).toString(16)
  if (hex.length < 2) hex = "0" + hex
  return hex
}

let hexToRgb = function (hex) {
    return hex.match(/[A-Za-z0-9]{2}/g).map(function (v) {
        return parseInt(v, 16)
    })
}

let fullColorHex = function (red, green, blue) {
  return '#' + rgbToHex(red) + rgbToHex(green) + rgbToHex(blue)
}

let overlayError = function (type) {
  let form = $('form')
  form.find('input, select, button, [data-toggle="buttons"]').attr('disabled', true)

  let page = $('#page')
  page.addClass('blur')

  if (type === 'connection') {
    $('body').append(`<div class="error-overlay"><div class="error-overlay-text">${$('[data-connection-error]').text()}</div></div>`)
  } else {
    $('body').append(`<div class="error-overlay"><div class="error-overlay-text">${$('[data-unexpected-error]').text()}</div></div>`)
  }
}

let updateForm = function (data) {
  $('#foregroundColor').val(fullColorHex(data.result.foregroundColor.red, data.result.foregroundColor.green, data.result.foregroundColor.blue))
  $('#backgroundColor').val(fullColorHex(data.result.backgroundColor.red, data.result.backgroundColor.green, data.result.backgroundColor.blue))
  $('#brightness').val(data.result.brightness).trigger('change')
  $('#timeZone').val(data.result.timeZone)
  $('#daylightSavingsTime').val(data.result.daylightSavingsTime)
  $('#daylightSavingsTimeHelper').prop('checked', data.result.daylightSavingsTime)
  $('#sleepTime').val(((data.result.sleepHour < 10) ? '0' + data.result.sleepHour : data.result.sleepHour) + ':' + ((data.result.sleepMinute < 10) ? '0' + data.result.sleepMinute : data.result.sleepMinute))
  $('#wakeupTime').val(((data.result.wakeupHour < 10) ? '0' + data.result.wakeupHour : data.result.wakeupHour) + ':' + ((data.result.wakeupMinute < 10) ? '0' + data.result.wakeupMinute : data.result.wakeupMinute))
  $('#language').val(data.result.language)
  $('#version').html(data.result.version)
}

let getSettings = function () {
  $.ajax({
    type: 'GET',
    url: 'settings',
    dataType: 'json',
    beforeSend: function () {
      $('form').find('input, select, button').attr('disabled', '')
    },
    success: function (data, textStatus, jqXHR) {
      if (data.success) {
        updateForm(data)
        $('form').find('input, select, button, [data-toggle="buttons"]').removeAttr('disabled')
      }
    },
    error: function (jqXHR, textStatus, errorThrown) {
      overlayError((jqXHR.status === 404 ? 'connection' : ''))
      console.log(errorThrown)
    }
  })
}

let setSettings = function (name, value) {
  let data = {}
  if (name == 'foregroundColor' || name == 'backgroundColor') {
      data[name] = hexToRgb(value)
  } else {
      data[name] = value
  }
  $.ajax({
    type: 'POST',
    url: 'update',
    data: JSON.stringify(data),
    dataType: 'json',
    accepts: 'application/json',
    contentType: 'application/x-www-form-urlencoded;charset=UTF-8',
    beforeSend: function () {
      $('form').find('input, select, button, [data-toggle="buttons"]').attr('disabled', true)
    },
    success: function (data, textStatus, jqXHR) {
      if (data.success) {
        $('form').find('input, select, button, [data-toggle="buttons"]').removeAttr('disabled')
      } else {
        overlayError((jqXHR.status === 404 ? 'connection' : ''))
      }
    },
    error: function (jqXHR, textStatus, errorThrown) {
      overlayError((jqXHR.status === 404 ? 'connection' : ''))
      console.log(errorThrown)
    }
  })
}

$(function () {

  getSettings()

  $('#brightness').on('mousemove touchmove change', function (event) {
    $('#brightnessValue').html($(this).val())
  })

  $('#timeZone').on('change', function (event) {
    let useDaylightTime = JSON.parse($(this).find(':selected').data('daylight-savings-time'))
    $('#daylightSavingsTime').val(useDaylightTime)
    $('#daylightSavingsTimeHelper').prop('checked', useDaylightTime)
  })

  $('#daylightSavingsTimeHelper').change(function (event) {
    if ($(this).is(':checked')) {
      $('#daylightSavingsTime').val(true)
    } else {
      $('#daylightSavingsTime').val(false)
    }
  })

  $('form').find('input, select, button, [data-toggle="buttons"]').on('change', event => {
      setSettings($(event.target).attr('name'), $(event.target).val())
      event.preventDefault()
  })
})
