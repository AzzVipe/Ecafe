<template>
	<main>
		<NavigationBar></NavigationBar>

		<div class="container">
			<div class="clients">
				<div class="pc-header">
					<router-link to='/' class="arrow-left">
						<i class="ri-arrow-left-line"></i>
					</router-link>
					<h1 class="pc-title">ECafe Desktop</h1>
				</div>
				
				<div class="pc-body">
					<PcBlock @refetch-client="fetchClient()" :client="client" :is-processing="isProcessing"></PcBlock>
				</div>

				<div class="clients-footer"></div>
			</div>
		</div>
	</main>
</template>

<script>

import NavigationBar from '../components/NavigationBar.vue';
import PcBlock from '../components/PcBlock.vue';
import axios from 'axios';

export default {
	name: "Pc",
	components: {
		NavigationBar,
		PcBlock,
	},
	props: ["id"],

	data() {
		return {
			client: {},
			isProcessing: true,
			actionTypes: ' ',
		};
	},

	methods: {
		fetchClient() {
			const url = `http://localhost:8080/client/show?id=${this.id}`;
			axios.get(url)
			.then((response) => {
				console.log(response);
				this.client = response.data.client;
				this.isProcessing = false;
			}).catch((error) => {
				console.log(error);
			});
		},
	},

	mounted() {
		this.fetchClient();	
		// setTimeout(()=>{
		// 	this.fetchClient();	
		// }, 2000)
	}
}


</script>