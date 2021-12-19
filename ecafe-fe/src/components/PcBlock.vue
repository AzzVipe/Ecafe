<template>
	<div class="pc-image">
		<div v-if="isImageLoading || isProcessing" class="loading loading-middle"></div>
		<img v-else id="image" :src="'/client/screenshot?id='+ client.id + '&v=' + v">
	</div>
	<div class="pc-body-title">{{client.username}}</div>
	<div class="param-keyvalue-body">
		<div class="param-keys">
			<p class="param-key">IP address:</p>
			<p class="param-key">Status:</p>
			<p class="param-key">Up time:</p>
		</div>
		<div class="param-values">
			<p class="param-value">{{ client.ip }}</p>
			<p
				class="param-value"
				:class="[client.status === 'online' ? 'is-active' : 'is-inactive']">
				<i class="ri-checkbox-blank-circle-fill" style="font-size: 8px"></i> {{ client.status }}
			</p>
			<p class="param-value">{{ upTime }}</p>
		</div>
	</div>
	<div class="pc-capsule-container">
		<div class="pc-capsule action-button">
			<div class="pc-select pc-capsule-left">
				<select name="actions" v-model="action" class="actions">

					<option value="0" selected="" disabled="">Action</option>
					<option v-for="a in getActions()" :value="a" :key="a.cmd">{{ a.name }}</option>

				</select>
				<i class="ri-arrow-down-s-line icon"></i>
			</div>
			<button @click="doAction()" type="submit" :class="{'is-loading': isPerformingAction}" class="button pc-capsule-right">
				<i class="ri-play-fill icon-play"></i>
			</button>
		</div>
	</div>
	<div v-if="showToast" class="toast" :class="[toastStatus ? 'is-success' : 'is-danger']">
		{{ msg }}
	</div>

</template>

<script>
	import { actions, performAction } from '../actions.js';
	export default {
		name: "PcBlock",
		props: ["client", "isProcessing"],
		emits: ['refetchClient'],

		data() {
			return {
				imgUrl: "/client/screenshot?id=",
				isImageLoading: false,
				screenshotImage: null,
				isPerformingAction: false,
				showToast: false,
				toastStatus: false,
				msg: "",
				action: "0",
				actions: actions,
				v: 0,
			};
		},

		methods: {
			getImage(data) {
				this.screenshotImage = data;
				let image = document.getElementById("image");
				console.log(image);
			},
			getActions() {
				const result = this.actions.filter((item) => {
					if (this.client.status === 'online' && item.cmd === 'unlock') {
						return false;
					} else if (this.client.status === 'offline' && item.cmd === 'lock') {
						return false;
					}

					return true;
				});
				
				return result;
			},

			doAction() {
				const param = new URLSearchParams();
				param.append('id', this.client.id);

				this.isPerformingAction = true;

				// if (this.action.overrideResponseHandler) {
				// 	this.action.onResponse = (action, response) => {
				// 		// @TODO Show image
				// 		console.log("SCREENSHOT", action, response);
				// 	}
				// }

				if (this.action.cmd === 'screenshot') {
					this.v += 1;
					this.isImageLoading = true;
					setTimeout(() => {
						this.isImageLoading = false;
					}, 500);
				}

				performAction(this.action, param).then((response) => {
					if (this.action.neadRefresh)
						this.$emit('refetchClient');

					if (!this.action.overrideResponseHandler && response.data.message) {
						this.toastStatus = true;
						this.msg = response.data.message;
					}

				}).catch((error) => {
					this.toastStatus = false;
					this.msg = error.data.message;
				}).finally(() => {
					if (this.action.canShowToast) {
						this.showToast = true;
						setTimeout(() => {
							this.showToast = false;
							this.msg = "";
						}, 4000);
					}
					this.action = "0";
					this.isPerformingAction = false;
					this.isImageLoading = false;
				});
			},

		},

		computed: {
			upTime() {
				if (this.client && !this.client.uptime) {
					return "";
				}

				const days = parseInt(this.client.uptime / 86400);
				const hours = parseInt((this.client.uptime / 3600) - (days * 24));
				const mins = parseInt((this.client.uptime / 60) - (days * 1440) - (hours * 60));


				return `${days}d:${hours}h:${mins}m`;
			}
		},
	}
</script>