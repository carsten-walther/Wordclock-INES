let setLanguage = function (language) {
  $.ajax({
    type: 'GET',
    url: 'i18n/' + language + '.json',
    dataType: 'json',
    success: function (data, textStatus, jqXHR) {
      if (Object.keys(data).length > 0) {
        processLanguage(data)
      }
    }
  })
}

let processLanguage = function (data) {
  $('legend,label,button,span,option').each(function () {
    if ($(this).data('langkey')) {
      $(this).text(data[$(this).data('langkey')])
    }
  })
}

$(function () {
  let language = (navigator.language || navigator.userLanguage).slice(0, 2)
  $('html').attr('lang', language)
  setLanguage(language)
})
