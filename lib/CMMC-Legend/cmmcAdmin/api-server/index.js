'use strict'

const Hapi = require('hapi')

// Create a server with a host and port
const server = new Hapi.Server()
server.connection({
  host: 'localhost',
  port: 8000
})

// Add the route
server.route(
  {
    method: ['GET', 'POST'],
    path: '/api/wifi/ap',
    handler: function (request, reply) {
      const method = request.method
      const payload = request.payload
      if (method === 'post') {
        console.log(payload)
        let json = {status: 'failed'}
        return reply(JSON.stringify(json))
      }
      else if (method === 'get') {
        let json = {'ssid': 'SSID FROM API', 'password': 'PASSWORD FROM API'}
        return reply(JSON.stringify(json))
      }
      else {
        return replay('not implemented.')
      }
    },
  })

server.route(
  {
    method: ['GET', 'POST'],
    path: '/api/mqtt',
    handler: function (request, reply) {
      console.log('/api/mqtt', request.payload)
      let ret = [{result: 'failed'}, {result: 'success', current: 'ESPERT-3020', ip: '192.168.1.1'}]
      let json = ret[Math.floor(Math.random(100) * 10) % 2]
      return reply(JSON.stringify(json))
    },
  })

server.route(
  {
    method: ['GET', 'POST'],
    path: '/api/wifi/sta',
    handler: function (request, reply) {
      let ret = [{result: 'failed'}, {result: 'success', current: 'ESPERT-3020', ip: '192.168.1.1'}]
      let json = ret[Math.floor(Math.random(100) * 10) % 2]
      return reply(JSON.stringify(json))
    },
  })

server.route(
  {
    method: ['GET', 'POST'],
    path: '/api/mqtt/config',
    handler: function (request, reply) {
      let ret = {
        h: 'odin.cmmc.io',
        port: '1883',
        usr: 'cmmc',
        pwd: 'cmmc',
        cid: 'clientId-RANDOM'
      }
      return reply(JSON.stringify(ret))
    },
  })

server.route(
  {
    method: ['GET'],
    path: '/api/wifi/scan',
    handler: function (request, reply) {
      let list = [
        [{name: 'Nat1'}, {name: ' ...@Pinn Creative Space 1'}],
        [{name: 'Jaylin Alexis'}, {name: 'Stanley'}],
        [{name: 'Denzel'}, {name: 'Evelyn'}],
        [{name: 'Sigmund Jaylin'}, {name: 'Christopher Clint'}],
        [{name: 'Baxter Windsor'}, {name: 'Windsor Clinton'}],
        [{name: 'Kurtis Barnaby Reginald'}, {name: 'Nevil James Baz'}],
        [{name: 'Bud Jemmy Paulie'}, {name: 'Julian Nigellus Devin'}]
      ]
      let idx = parseInt((Math.random() * 100)) % list.length
      return reply(JSON.stringify(list[idx]))
    },
  })

// Start the server
server.start((err) => {

  if (err) {
    throw err
  }
  console.log('Server running at:', server.info.uri)
})
