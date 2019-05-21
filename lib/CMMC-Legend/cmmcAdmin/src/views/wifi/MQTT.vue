<template>

  <div class="content">
    <section class="section">
      <div class="columns">
        <div class="column">
          <div class="heading">
            <h1 class="title">MQTT CONFIGURATION</h1>
          </div>
          <div v-if="server_response" class="notification is-primary">
            {{ server_response }}
          </div>
          <label class="label">Host</label>
          <p class="control has-icon">
            <input class="input" type="text" v-model="host" required>
            <i class="fa fa-connectdevelop"></i>
          </p>
          <label class="label">Port</label>
          <p class="control has-icon">
            <input class="input" type="number" v-model="port" required>
            <i class="fa fa-exchange"></i>
          </p>
          <label class="label">ClientID</label>
          <p class="control has-icon">
            <input class="input" type="text" v-model="clientId">
            <i class="fa fa-vcard-o"></i>
          </p>
          <label class="label">Username</label>
          <p class="control has-icon">
            <input class="input" type="text" v-model="username">
            <i class="fa fa-user"></i>
          </p>
          <label class="label">Password</label>
          <p class="control has-icon">
            <input class="input" type="password" v-model="password">
            <i class="fa fa-lock"></i>
          </p>
          <label class="label">Prefix</label>
          <p class="control has-icon">
            <input class="input" type="text" v-model="prefix">
            <i class="fa fa-child"></i>
          </p>
          <label class="label">Device Name</label>
          <p class="control has-icon">
            <input class="input" type="text" v-model="deviceName" required>
            <i class="fa fa-child"></i>
          </p>
          <label class="label">Publish Eevery Second</label>
          <p class="control has-icon">
            <input class="input" type="number" v-model="publishRateSecond" required>
            <i class="fa fa-exchange"></i>
          </p>
          <div class="control">
            <input type="checkbox" v-model="lwt"> enable LWT
          </div>
          <div class="control">
            <button class="button is-primary" v-on:click.stop="onSubmit">Submit</button>
            <button class="button is-link">Cancel</button>
          </div>
        </div>
      </div>
    </section>
  </div>

</template>

<script>
  import { saveMqttConfig, getMqttConfig } from '../../api'

  export default {
    components: {},
    mounted () {
      const hash = `${Math.random().toString(15).substr(2, 10)}`
      getMqttConfig(this).then((json) => {
        this.host = json.host
        this.username = json.username
        this.password = json.password
        this.clientId = json.clientId || `clientId-${hash}`
        this.publishRateSecond = json.publishRateSecond
        this.deviceName = json.deviceName || hash
        this.prefix = json.prefix
        this.lwt = json.lwt
        this.port = json.port
      }).catch((err) => {
        console.log('error:', err)
      })
    },
    methods: {
      onSubmit () {
        let context = this
        saveMqttConfig(context, {
          host: context.host,
          username: context.username,
          password: context.password,
          prefix: context.prefix || '',
          clientId: context.clientId || `clientId-${context.hash}`,
          publishRateSecond: context.publishRateSecond,
          deviceName: context.deviceName || context.hash,
          lwt: (context.lwt === false) ? '0' : '1',
          port: context.port
        }).then((resp) => {
          this.server_response = 'Saved.'
          this.fetchConfig()
        }).catch((err) => {
          console.log('error', err)
        })
      },
      fetchConfig () {
        let ctx = this
        const hash = `${Math.random().toString(15).substr(2, 10)}`
        getMqttConfig(ctx).then((configs) => {
          configs.deviceName = configs.deviceName || hash
          configs.clientId = configs.clientId || hash
          Object.entries(configs).forEach(([key, value]) => {
            console.log(`config ${key}=>${value}`)
            ctx[key] = value
          })
        })
      }
    },
    data () {
      const hash = `${Math.random().toString(15).substr(2, 10)}`
      return {
        hash,
        server_response: '',
        username: '',
        password: '',
        deviceName: hash,
        lwt: 0,
        clientId: `clientId-${hash}`,
        host: 'mqtt.cmmc.io',
        publishRateSecond: 60,
        port: 1883,
        prefix: ''
      }
    },
    created () {
      console.log('on created.')
      let ctx = this
    }
  }
</script>

<style scoped>
  .button {
    margin: 5px 0 0;
  }

  .control .button {
    margin: inherit;
  }
</style>
