import './assets/main.css'

import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import { ElButton } from 'element-plus';
import 'element-plus/theme-chalk/dark/css-vars.css'

const app = createApp(App)

app.use(router)
app.component('ElButton',ElButton)


app.mount('#app')
