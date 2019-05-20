<template>
  <div class="content">
    <section class="section">
      <div class="columns">
        <div class="column">
          <div class="heading">
            <h1 class="title">STA Configuration</h1>
          </div>
          <div v-if="server_response" class="notification is-primary">
            {{ server_response }}
          </div>
          <label class="label">Manual SSID</label>
          <p class="control has-icon">
            <input v-model="sta_ssid" class="input" type="text" placeholder="SSID">
            <i class="fa fa-wifi"></i>
          </p>
          <label class="label">Password</label>
          <p class="control has-icon">
            <input v-model="sta_password" class="input" type="password" placeholder="Password">
            <i class="fa fa-lock"></i>
          </p>
          <p class="control">
            <button class="button is-primary" v-on:click.stop="onSubmit">Submit</button>
            <button class="button is-link">Cancel</button>
          </p>
        </div>
      </div>
    </section>
  </div>
</template>

<script>
  import { saveSTAConfig, getSTAConfig } from '../../api'

  export default {
    components: {},
    props: {},
    mounted () {
      getSTAConfig(this).then((json) => {
        this.sta_ssid = json.sta_ssid
        this.sta_password = json.sta_password
      })
        .catch((err) => {
          console.log('error:', err)
        })
    },
    methods: {
      onSubmit () {
        let context = this
        saveSTAConfig(context, context.sta_ssid, context.sta_password)
          .then((resp) => {
            context.server_response = JSON.stringify(resp)
            context.saving = false
          })
          .catch((err) => {
            context.saving = false
            console.log(err)
          })
      },
    },
    data () {
      return {
        server_response: '',
        sta_ssid: null,
        sta_password: null
      }
    },
    computed: {
      now: function () {
        return Date.now()
      }
    },
    created () {

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
