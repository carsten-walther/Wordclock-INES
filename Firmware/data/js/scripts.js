
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
  form.find('input, select, button, [data-toggle="buttons"]').attr('disabled', true);

  let page = $('#page');
  page.addClass('blur');

  if (type === 'connection') {
    $('body').append(`<div class="error-overlay"><div class="error-overlay-text">${form.data('connection-error')}</div></div>`);
  } else {
    $('body').append(`<div class="error-overlay"><div class="error-overlay-text">${form.data('unexpected-error')}</div></div>`);
  }
}

let updateForm = function (data) {
  $('#version').html(data.result.version);
  $('#foregroundColor').val(fullColorHex(data.result.foregroundColor.red, data.result.foregroundColor.green, data.result.foregroundColor.blue));
  $('#backgroundColor').val(fullColorHex(data.result.backgroundColor.red, data.result.backgroundColor.green, data.result.backgroundColor.blue));
  $('#brightness').val(data.result.brightness).trigger('change');
  $('#timeZone').val(data.result.timeZone);
  $('#daylightSavingsTime').prop('checked', data.result.daylightSavingsTime);
  $('#sleepTime').val(((data.result.sleepHour < 10) ? '0' + data.result.sleepHour : data.result.sleepHour) + ':' + ((data.result.sleepMinute < 10) ? '0' + data.result.sleepMinute : data.result.sleepMinute));
  $('#wakeupTime').val(((data.result.wakeupHour < 10) ? '0' + data.result.wakeupHour : data.result.wakeupHour) + ':' + ((data.result.wakeupMinute < 10) ? '0' + data.result.wakeupMinute : data.result.wakeupMinute));
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
      if (data.success) {
        updateForm(data);
        $('form').find('input, select, button, [data-toggle="buttons"]').removeAttr('disabled');
      }
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
      $('form').find('input, select, button, [data-toggle="buttons"]').attr('disabled', true);
    },
    success: function (data, textStatus, jqXHR) {
      if (data.success) {
        updateForm(data);
        $('form').find('input, select, button, [data-toggle="buttons"]').removeAttr('disabled');
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

  $('#brightness').on('mousemove touchmove change', function (event) {
    $('#brightnessValue').html($(this).val());
  });

  $('#daylightSavingsTimeHelper').change(function (event) {
    if ($(this).is(':checked')) {
      $('#daylightSavingsTime').val(true);
    } else {
      $('#daylightSavingsTime').val(false);
    }
  });

  $('#submit').click(function (event) {
    event.preventDefault();
    setSettings();
  });

});
