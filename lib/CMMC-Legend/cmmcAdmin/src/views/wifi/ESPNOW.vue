<template>
  <div class="content">
    <section class="section">
      <div class="columns">
        <div class="column">
          <div class="heading">
            <h1 class="title">ESP Now Configuration</h1>
          </div>
          <label class="label">Target Mac Address</label>
          <div v-if="server_response" class="notification is-primary">
            {{ server_response }}
          </div>
          <p class="control has-icon">
            <input class="input" type="text" v-model="mac">
            <i class="fa fa-space-shuttle"></i>
          </p>
          <label class="label">Current Mac Address</label>
          <p class="control has-icon">
            <input class="input" type="text" v-model="self_mac" disabled>
            <i class="fa fa-car"></i>
          </p>
          <label class="label">Device Name</label>
          <p class="control has-icon">
            <input class="input" type="text" v-model="deviceName">
            <i class="fa fa-angellist"></i>
          </p>
          <!--<label class="label">Mode</label>-->
          <!--<div class="control">-->
          <!--<label class="radio">-->
          <!--<input type="radio" name="answer" v-model="mode" value="master">-->
          <!--Master-->
          <!--</label>-->
          <!--<label class="radio">-->
          <!--<input type="radio" name="answer" v-model="mode" value="slave">-->
          <!--Slave-->
          <!--</label>-->
          <!--</div>-->
          <p class="control">
            <button class="button is-primary" v-on:click="onSubmit">Submit</button>
            <button class="button is-link">Cancel</button>
          </p>
        </div>
      </div>
    </section>
  </div>
</template>

<script>
  export default {
    components: {},

    props: {},
    mounted () {
      this.loadSensor()
    },
    methods: {
      loadSensor () {
        const context = this
        context.$http.get('/espnow.json')
          .then((response) => response.json())
          .then((json) => {
            context.mac = json.mac
            context.self_mac = json.self_mac
            context.deviceName = json.deviceName
            // context.server_response = JSON.stringify(json)
          })
          .catch((err) => {
            console.log(err)
          })
      },
      onSubmit () {
        let context = this
        let formData = new window.FormData()
        formData.append('mac', context.mac)
        formData.append('deviceName', context.deviceName)
        context.$http.post('/api/espnow', formData)
          .then((response) => response.json())
          .then((json) => {
            context.server_response = 'ESPNow configurations has been saved!'
            context.loadSensor()
          })
          .catch((err) => {
            console.log(err)
          })
      }
    },
    data () {
      return {
        server_response: null,
        mac: '',
        self_mac: '',
        deviceName: '',
      }
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
