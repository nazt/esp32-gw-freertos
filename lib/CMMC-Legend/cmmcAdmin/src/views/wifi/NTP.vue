<template>

  <div class="content">
    <section class="section">
      <div class="columns">
        <div class="column">
          <div class="heading">
            <h1 class="title">MQTT CONFIGURATION</h1>
          </div>
          <label class="label">Host</label>
          <p class="control has-icon">
            <input class="input" type="text" placeholder="example mqtt.cmmc.io">
            <i class="fa fa-connectdevelop"></i>
          </p>
          <label class="label">Port</label>
          <p class="control has-icon">
            <input class="input" type="text">
            <i class="fa fa-exchange"></i>
          </p>
          <label class="label">ClientID</label>
          <p class="control has-icon">
            <input class="input" type="text" v-bind:value="clientId">
            <i class="fa fa-vcard-o"></i>
          </p>
          <label class="label">Username</label>
          <p class="control has-icon">
            <input class="input" type="text" placeholder="Username">
            <i class="fa fa-user"></i>
          </p>
          <label class="label">Password</label>
          <p class="control has-icon">
            <input class="input" type="password" placeholder="Password">
            <i class="fa fa-lock"></i>
          </p>
          <div class="control">
            <button class="button is-primary">Submit</button>
            <button class="button is-link">Cancel</button>
          </div>
        </div>
      </div>
    </section>
  </div>

</template>

<script>
  export default {
    components: {},
    data () {
      return {
        mqtt_clientId: `clientId-${Math.random().toString(15).substr(2, 10)}`
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
