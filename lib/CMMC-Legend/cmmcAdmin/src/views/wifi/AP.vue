<template>
  <div class="content">
    <section class="section">
      <div class="columns">
        <div class="column">
          <div class="heading">
            <h1 class="title">AP Configuration</h1>
          </div>
          <div v-if="server_response" class="notification is-primary">
            {{ server_response }}
          </div>
          <label class="label">AP</label>
          <p class="control has-icon">
            <input disabled v-model="ap_ssid" class="input" type="text">
            <i class="fa fa-wifi"></i>
          </p>
          <label class="label">Password</label>
          <p class="control has-icon">
            <input disabled v-model="ap_password" class="input" type="password">
            <i class="fa fa-lock"></i>
          </p>
          <p class="control">
            <button disabled class="button is-primary" v-on:click.stop="onSubmit">Submit</button>
            <button disabled class="button is-link">Cancel</button>
          </p>
        </div>
      </div>
    </section>
  </div>
</template>

<script>
  import { saveAPConfig, getAPConfig } from '../../api'

  export default {
    components: {},

    props: {},

    mounted () {
      getAPConfig(this).then((json) => {
        this.ap_ssid = json.ap_ssid
        this.ap_password = json.ap_password
      })
        .catch((err) => {
          console.log('error:', err)
        })
    },
    methods: {
      onSubmit () {
        let context = this
        saveAPConfig(context, context.ap_ssid, context.ap_password)
          .then((resp) => {
            this.server_response = 'Saved.'
          })
          .catch((err) => {
            console.log('error', err)
          })
      }
    },
    data () {
      return {
        server_response: null,
        post: {},
        ap_ssid: '',
        ap_password: ''
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
