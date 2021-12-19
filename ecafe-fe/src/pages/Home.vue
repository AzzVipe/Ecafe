<template>
	<main>
		<NavigationBar></NavigationBar>
		<div class="container">
			<div class="clients">
				<ClientHeader v-model="q"></ClientHeader>
				<TaskBar 
					:n-clients="nClients" 
					:n-online="nOnline"
					:n-offline="nOffline"
					:actions="actions"
					@fire-action="fireAction"
				>
						
				</TaskBar>
				<div class="clients-body">

					<ClientBlock 
						@lock-client="doAction(lockAction, [client])" 
						@unlock-client="doAction(unlockAction, [client])"
						v-for="(client, index) in sortedClients" 
						:client="client" :key="index"
					/>

				</div>

				<div class="clients-footer"></div>
				<div v-if="showToast && msg" :class="{'is-success': toastStatus, 'is-danger': !toastStatus}" class="toast">
					<p>{{ msg }}</p>
				</div>
			</div>
		</div>
	</main>
</template>

<script>

import NavigationBar from '../components/NavigationBar.vue';
import ClientHeader from '../components/ClientHeader.vue';
import ClientBlock from '../components/ClientBlock.vue';
import TaskBar from '../components/TaskBar.vue';
import axios from 'axios';
import { actions, performAction } from '../actions.js';

export default {
	name: "Home",
	components: {
		NavigationBar,
		ClientHeader,
		ClientBlock,
		TaskBar,
	},

	data() {
		return {
			clients: [],
			sortedClients: [],
			actions: actions,
			lockAction: null,
			unlockAction: null,

			showToast: false,
			toastStatus: false,
			msg: "",
			q: "",
		};
	},

	computed: {
		nClients() {
			return this.sortedClients.length;
		},

		nOnline() {
			return this.sortedClients.filter(obj => obj.status === 'online').length ;
		},

		nOffline() {
			return this.sortedClients.filter(obj => obj.status === 'offline').length ;
		}
	},

	methods: {
		fetchClients() {
			const url = "http://localhost:8080/client/index";
			axios.get(url)
			.then((response) => {
				console.log(response);
				this.clients = response.data.clients;
				this.sortedClients = this.clients;
			});
		},

		fireAction(action) {
			this.doAction(action, this.clients);
		},

		doAction(action, clients) {
			clients.forEach((client) => {
				const param = new URLSearchParams();
				param.append('id', client.id);

				performAction(action, param).then((response) => {
					this.toastStatus = true;
					this.msg = response.data.message;
					if (action.neadRefresh)
						this.fetchClients();
			
				}).catch((error) => {
					this.toastStatus = false;
					this.msg = error.data.message;
				}).finally(() => {
					this.showToast = true;
					setTimeout(() => {
						this.showToast = false;
						this.msg = "";
					}, 4000);
				});
			});
		},
	},

	mounted() {
		this.fetchClients();
		this.actions.forEach((action) =>{
			if (action.cmd === 'lock') {
				this.lockAction = action;
			}
			else if(action.cmd === 'unlock') {
				this.unlockAction = action;
			}
		});
	},

	watch: {
		q(new_value, old_value) {
			console.log(new_value, old_value);
			this.sortedClients = this.clients.filter(client => client.username.includes(new_value));
			console.log(this.sortedClients);
		}
	}
}
</script>