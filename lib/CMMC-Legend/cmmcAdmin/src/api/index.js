import Vue from 'vue'

export function getAPConfig (context) {
  let promise = new Vue.Promise((resolve, reject) => {
    context.$http.get('/wifi.json')
      .then((response) => response.json())
      .then((json) => {
        resolve(json)
      })
      .catch((err) => {
        console.log(err)
      })
  })
  return promise
}

export function getConfig (context, type = 'mqtt') {
  let promise = new Vue.Promise((resolve, reject) => {
    context.$http.get(`/api/${type}/config`)
      .then((response) => response.json())
      .then((json) => {
        resolve(json)
      })
      .catch((err) => {
        console.log(err)
      })
  })
  return promise
}

export function saveAPConfig (context, ssid, pass) {
  var formData = new window.FormData()
  formData.append('ap_ssid', ssid)
  formData.append('ap_password', pass)
  let promise = new Vue.Promise((resolve, reject) => {
    context.$http.post('/api/wifi/ap', formData)
      .then((response) => response.json())
      .then((json) => {
        resolve(json)
      })
      .catch((err) => {
        console.log(err)
      })
  })
  return promise
}

export function getMqttConfig (context) {
  let promise = new Vue.Promise((resolve, reject) => {
    context.$http.get('/mymqtt.json')
      .then((response) => response.json())
      .then((json) => {
        resolve(json)
      })
      .catch((err) => {
        console.log(err)
      })
  })
  return promise
}

export function saveMqttConfig (context, options) {
  const formData = new window.FormData()
  formData.append('host', options.host)
  formData.append('username', options.username)
  formData.append('password', options.password)
  formData.append('deviceName', options.deviceName)
  formData.append('clientId', options.clientId)
  formData.append('prefix', options.prefix)
  formData.append('port', options.port)
  formData.append('lwt', options.lwt)
  formData.append('publishRateSecond', options.publishRateSecond)
  let promise = new Vue.Promise((resolve, reject) => {
    context.$http.post('/api/mqtt', formData)
      .then((response) => response.json())
      .then((json) => {
        resolve(json)
      })
      .catch((err) => {
        console.log(err)
      })
  })
  return promise
}

export function getSTAConfig (context) {
  let promise = new Vue.Promise((resolve, reject) => {
    context.$http.get('/wifi.json')
      .then((response) => response.json())
      .then((json) => {
        resolve(json)
      })
      .catch((err) => {
        console.log(err)
      })
  })
  return promise
}

export function saveSTAConfig (context, ssid, password) {
  const formData = new window.FormData()
  formData.append('sta_ssid', ssid)
  formData.append('sta_password', password)
  let promise = new Vue.Promise((resolve, reject) => {
    context.$http.post('/api/wifi/sta', formData)
      .then((response) => response.json())
      .then((json) => {
        resolve(json)
      })
      .catch((err) => {
        console.log(err)
      })
  })
  return promise
}

export function getESPNowConfig (context) {
  let promise = new Vue.Promise((resolve, reject) => {
    context.$http.get('/api/espnow')
      .then((response) => response.json())
      .then((json) => {
        resolve(json)
      })
      .catch((err) => {
        console.log(err)
      })
  })
  return promise
}

export function saveESPNowConfig (context, options) {
  const formData = new window.FormData()
  formData.append('esp_now_mac_address', options.macAddress)
  formData.append('esp_now_mode', options.mode)
  let promise = new Vue.Promise((resolve, reject) => {
    context.$http.post('/api/espnow', formData)
      .then((response) => response.json())
      .then((json) => {
        resolve(json)
      })
      .catch((err) => {
        console.log(err)
      })
  })
  return promise
}

export function getAccessPoints (context) {
  // to be tested
  let promise = new Vue.Promise((resolve, reject) => {
    let list = []
    let fin = []
    let out = {}
    console.log('calling...')
    context.$http.get('/api/wifi/scan').then((response) => response.json())
      .then((aps) => {
        console.log(aps)
        list.push(aps)
        for (let _aps of list) {
          _aps.forEach((v, k) => {
            out[v.name] = k
          })
        }
        for (let it of Object.keys(out)) {
          fin.push({name: it})
        }
        resolve(fin)
      })
      .catch((error) => {
        console.log('eroro', error.status, error)
        reject(error)
      })
  })

  return promise
}

export function saveWiFiConfig (context, ssid, pass) {
  const formData = new window.FormData()
  formData.append('sta_ssid', ssid)
  formData.append('sta_password', pass)
  return new Vue.Promise((resolve, reject) => {
    context.$http.post('/api/wifi/sta', formData)
      .then((response) => response.json())
      .then((response) => {
        console.log('resp=', response)
        resolve(response)
      }).catch((error) => {
      reject(error)
    })
  })
}
