
let rgbToHex = function (rgb) {
  let hex = Number(rgb).toString(16);
  if (hex.length < 2) hex = "0" + hex;
  return hex;
};

let fullColorHex = function (red, green, blue) {
  return '#' + rgbToHex(red) + rgbToHex(green) + rgbToHex(blue);
};

let overlayError = function (type) {
  let form = $('form');
  form.find('input, select, button').attr('disabled', '');

  let page = $('#page');
  page.addClass('blur');

  if (type === 'connection') {
    $('body').append(`<div class="error-overlay"><div class="error-overlay-text">${form.data('connection-error')}</div></div>`);
  } else {
    $('body').append(`<div class="error-overlay"><div class="error-overlay-text">${form.data('unexpected-error')}</div></div>`);
  }
}

let getSettings = function () {
  $.ajax({
    type: 'GET',
    url: 'settings.json',
    dataType: 'json',
    beforeSend: function () {
      $('form').find('input, select, button').attr('disabled', '');
    },
    success: function (data, textStatus, jqXHR) {

      $('#version').html(data.version);
      $('#foregroundColor').val(fullColorHex(data.foregroundColor.red, data.foregroundColor.green, data.foregroundColor.blue));
      $('#backgroundColor').val(fullColorHex(data.backgroundColor.red, data.backgroundColor.green, data.backgroundColor.blue));
      $('#brightness').val(data.brightness);
      $('#timeZone').val(data.timeZone);
      $('#daylightSavingsTime').prop('checked', data.daylightSavingsTime);
      $('#sleepTime').val(((data.sleepHour < 10) ? '0' + data.sleepHour : data.sleepHour) + ':' + ((data.sleepMinute < 10) ? '0' + data.sleepMinute : data.sleepMinute));
      $('#wakeupTime').val(((data.wakeupHour < 10) ? '0' + data.wakeupHour : data.wakeupHour) + ':' + ((data.wakeupMinute < 10) ? '0' + data.wakeupMinute : data.wakeupMinute));

      $('form').find('input, select, button').removeAttr('disabled');
    },
    error: function (jqXHR, textStatus, errorThrown) {
      overlayError((jqXHR.status === 404 ? 'connection' : ''));
      console.log(errorThrown);
    }
  });
}

let setSettings = function () {
  $.ajax({
    type: 'POST',
    url: 'update.json',
    data: $('form').serialize(),
    dataType: 'json',
    beforeSend: function () {
      $('form').find('input, select, button').attr('disabled', '');
    },
    success: function (data, textStatus, jqXHR) {
      if (data.success) {
        getSettings();
      } else {
        overlayError((jqXHR.status === 404 ? 'connection' : ''));
      }
    },
    error: function (jqXHR, textStatus, errorThrown) {
      overlayError((jqXHR.status === 404 ? 'connection' : ''));
      console.log(errorThrown);
    }
  });
}

$(function () {

  getSettings();

  $('#submit').click(function (event) {
    event.preventDefault();
    setSettings();
  });

});
