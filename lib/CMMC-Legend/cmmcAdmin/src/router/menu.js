// show: meta.label -> name
// name: component name
// meta.label: display label

// lazy loading Components
// https://github.com/vuejs/vue-router/blob/dev/examples/lazy-loading/app.js#L8
const lazyLoading = (name, index = false) => () => System.import(`views/${name}${index ? '/index' : ''}.vue`)

export default [
  {
    name: 'WiFi Configuration',
    path: '/wifi',
    meta: {
      icon: 'fa-wifi',
      expanded: true
    },
    component: require('../views/blank'),

    children: [
      {
        name: 'AP',
        path: 'ap',
        component: require('../views/wifi/AP')
      },
      {
        name: 'STA',
        path: 'sta',
        component: require('../views/wifi/STA')
      }
    ]
  },
  {
    name: 'Configurations',
    path: '/config',
    meta: {
      icon: 'fa-life-saver',
      expanded: true
    },
    component: require('../views/blank'),
    children: [
      // {
      //   name: 'NTP',
      //   path: 'general',
      //   component: require('../views/wifi/NTP')
      // },
      {
        name: 'MQTT',
        path: 'mqtt',
        component: require('../views/wifi/MQTT')
      }
    ]
  },
  {
    name: 'Sensors',
    path: '/sensors',
    meta: {
      icon: 'fa-puzzle-piece',
      expanded: false
    },
    component: require('../views/sensors/ALL'),
    children: [
      // {
      //   name: 'Sensor Activation',
      //   path: 'type',
      //   component: require('../views/sensors/ALL')
      // },
      // {
      //   name: 'DHT',
      //   path: 'dht',
      //   component: require('../views/sensors/DHT')
      // },
      // {
      //   name: 'BME',
      //   path: 'bme',
      //   component: require('../views/sensors/BME')
      // }
    ]
  },
  {
    name: 'OTA',
    path: '/ota',
    meta: {
      icon: 'fa-bank',
      expanded: false
    },
    component: require('../views/blank'),
    children: []
  },
  {
    name: 'Deep Sleep',
    path: '/deep',
    meta: {
      icon: 'fa-beer',
      expanded: false
    },
    component: require('../views/blank'),
    children: []
  },
  {
    name: 'ESP Now',
    path: '/espnow',
    meta: {
      icon: 'fa-fire',
      expanded: false
    },
    component: require('../views/wifi/ESPNOW'),
    children: []
  }
]
