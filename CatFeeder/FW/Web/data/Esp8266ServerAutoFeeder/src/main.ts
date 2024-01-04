import './assets/main.css'

import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import { ElButton } from 'element-plus';

const app = createApp(App)

app.use(router)
app.component('ElButton',ElButton)


app.mount('#app')
