<template>
  <div class="content">
    <section class="section">
      <div class="columns">
        <div class="column">
          <div class="heading">
            <h1 class="title">Sensor Configuration</h1>
          </div>
          <div v-if="server_response" class="notification is-primary">
            {{ server_response }}
          </div>
          <label class="label">Sensors</label>
          <div class="control">
            <input type="radio" id="BME280" value="BME280" v-model="sensorType">
            <label for="BME280">BME 280</label>
            <br>
            <input type="radio" id="BME680" value="BME680" v-model="sensorType">
            <label for="BME680">BME 680</label>
            <br>
            <input type="radio" id="DHT11" value="DHT11" v-model="sensorType">
            <label for="DHT11">DHT 11</label>
            <br>
            <input type="radio" id="DHT22" value="DHT22" v-model="sensorType">
            <label for="DHT22">DHT 22</label>
            <br>
            <input type="radio" id="NO_SENSOR" value="NO_SENSOR" v-model="sensorType">
            <label for="DHT22">No Sensor</label>
            <br>
          </div>

          <div class="control">
            <label class="label">BME I2C Address</label>
            <p class="control has-icon">
              <input class="input" placeholder="BME Address" value="" disabled/>
              <i class="fa fa-thermometer"></i>
            </p>
          </div>


          <div class="control">
            <label class="label">DHT PIN</label>
            <p class="control has-icon">
              <input class="input" placeholder="Pin Number" v-model="dht_pin"/>
              <i class="fa fa-thermometer"></i>
            </p>
          </div>


          <p class="control">
            <button class="button is-primary" v-on:click="onSubmit">Activate</button>
            <button class="button is-link">Cancel</button>
          </p>
        </div>
      </div>
    </section>
    <router-view></router-view>
  </div>

</template>


<script>
  export default {
    components: {},

    props: {},

    methods: {
      loadSensor () {
        const context = this
        context.$http.get('/sensors.json')
          .then((response) => response.json())
          .then((json) => {
            context.bme_pin = json.bme_pin
            context.bme_addr = json.bme_addr
            context.sensorType = json.sensorType
            // context.server_response = JSON.stringify(json)
          })
          .catch((err) => {
            console.log(err)
          })
      },
      onSubmit () {
        let context = this
        let formData = new window.FormData()
        formData.append('bme_pin', `${context.bme_pin}`)
        formData.append('bme_addr', '0x77')
        formData.append('dht_pin', context.dht_pin)
        formData.append('sensorType', context.sensorType)
        context.$http.post('/api/sensors/config', formData)
          .then((response) => response.json())
          .then((json) => {
            context.server_response = 'Saved'
            context.loadSensor()
          })
          .catch((err) => {
            console.log(err)
          })
      }
    },
    data () {
      return {
        loading: false,
        server_response: '',
        dht_pin: '12',
        bme_pin: '',
        bme_addr: '',
        sensorType: 'BME680'
      }
    },

    mounted () {
      console.log('mounted')
      this.loadSensor()
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
