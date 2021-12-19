import { createApp } from 'vue'
import { createRouter, createWebHashHistory } from 'vue-router'
import App from './App.vue'

import Home from './pages/Home.vue';
import Pc from './pages/Pc.vue';
import User from './pages/User.vue';

const routes = [
  { path: '/', component: Home },
  { path: '/pc/:id', component: Pc, props: true },
  { path: '/user', component: User},
];


const router = createRouter({
  // 4. Provide the history implementation to use. We are using the hash history for simplicity here.
  history: createWebHashHistory(),
  routes, // short for `routes: routes`
});


const app = createApp(App);

app.use(router);
app.mount('#app');