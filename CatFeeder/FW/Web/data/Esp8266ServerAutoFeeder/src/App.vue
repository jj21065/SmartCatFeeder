<script setup lang="ts">
import { RouterLink, RouterView } from 'vue-router'
import HelloWorld from './components/HelloWorld.vue'
</script>

<template>
  <header>
    <!-- <img alt="Vue logo" class="logo" src="@/assets/logo.svg" width="125" height="125" /> -->
    <span class = "logoBox" >
       <div class="logoBackground" >
      <img alt="Vue logo" class="logo" src="@/assets/catLogo_pic.png" width="145" height="145" />
    </div>
    </span>
   
    <div class="wrapper">
      <HelloWorld msg="HooHoo Feeder!" />

      <nav>
        <RouterLink to="/">Home</RouterLink>
        <RouterLink to="/about">About</RouterLink>
      </nav>
    </div>
  </header>

  <RouterView />
</template>

<style scoped>
header {
  line-height: 1.5;
  max-height: 100vh;
}

.logo {
  display: block;
  margin-top:-12px;
}

nav {
  width: 100%;
  font-size: 12px;
  text-align: center;
  margin-top: 2rem;
}

nav a.router-link-exact-active {
  color: var(--color-text);
}

nav a.router-link-exact-active:hover {
  background-color: transparent;
}

nav a {
  display: inline-block;
  padding: 0 1rem;
  border-left: 1px solid var(--color-border);
}

nav a:first-of-type {
  border: 0;
}

div.logoBackground{
  border-radius: 50%;
  /* position: absolute; */
  background-color: grey;
  width: 115px;
  height: 115px;
  display: flex;
  justify-content: center;
  /* margin: 0 auto 2rem; */
}
.logoBox{
  display: flex;
  justify-content: center;
}


@media (min-width: 1024px) {
  header {
    display: flex;
    place-items: center;
    padding-right: calc(var(--section-gap) / 2);
  }

  .logo {
    /* margin: 0 2rem 0 0; */
    position: relative;
    z-index:20;
  }

  header .wrapper {
    display: flex;
    place-items: flex-start;
    flex-wrap: wrap;
  }

  nav {
    text-align: left;
    margin-left: -1rem;
    font-size: 1rem;

    padding: 1rem 0;
    margin-top: 1rem;
  }
.logoBox{
  display: flex;
  justify-content: center;
  margin-right: 2rem;
    margin-top:-12px;

}
}
</style>
