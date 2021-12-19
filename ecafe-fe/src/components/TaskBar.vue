<template>

	<div class="taskbar-container">
		<div class="counts-container">
			<label class="checkbox">
				<input type="checkbox" id="is_fixed" name="is_fixed">
				<span class="checkbox-button"></span>
			</label>

			<div class="status-box">
				<p v-if="nClients <= 1"> {{ nClients }} Client</p>
				<p v-else >{{ nClients }} Clients</p>
			</div>
			<div class="status-box online-box">
				<p> {{ nOnline }} Online</p>
			</div>

			<div class="status-box offline-box">
				<p> {{ nOffline }} Offline</p>
			</div>
		</div>

		<div class="taskbar-menu-icon has-status-online mode-light">
			<!-- <router-link to='/user' style="width: 20px; display: block;">
				<i class="ri-menu-line taskbar-menu-icon"></i>
			</router-link> -->
			<i class="ri-menu-line"></i>
			<div class="dropdown">
				<div class="dropdown-wrapper">
					<div class="dropdown-menu">
						<a v-for="a in filteredActions" @click="$emit('fire-action', a)" :key="a.cmd" class="dropdown-menu-item">{{ a.name }}</a>
						<!-- <a v @click="$emit('fire-action', 'unlock')" class="dropdown-menu-item">Unlock</a>
						<a v @click="$emit('fire-action', 'ping')" class="dropdown-menu-item">Ping</a>
						<a v @click="$emit('fire-action', 'poweroff')" class="dropdown-menu-item">Shutdown</a> -->
					</div>
				</div>
			</div>
		</div>
	</div>

</template>

<script>

export default {
	name: "TaskBar",
	props: ['nClients', 'nOnline', 'nOffline', 'actions'],
	emits: ['fire-action'],

	computed: {
		filteredActions() {
			return this.actions.filter((action) => action.massActionable);
		}
	},

}

</script>

<style>
	.taskbar-container {
		display: flex;
		margin: 1rem;
		justify-content: space-between;
	}

	.counts-container {
		display: flex;
		align-items: center;
	}

	.status-box {
		margin-left: .75rem;
		display: flex;
		text-align: center;
		justify-content: center;
		align-items: center;
		/*height: 20px;*/
		border-radius: 4px;
		background-color: #111013;
		padding: .25rem .75rem;
		color: #888888;
	}
	.status-box p {
		font-size: .75rem;
	}

	.online-box {
		color: #67A771;
	}

	.offline-box {
		color: #B95B5B;
	}

	.taskbar-menu-icon {
		color: #888;
		cursor: pointer;
		font-size: 20px;
	}

	.checkbox {
		display: inline-flex;
		align-items: center;
		height: 20px;
	}

	.checkbox input[type="checkbox"] {
		width: 0px;
		height: 0px;
		visibility: hidden;
	}

	.checkbox .checkbox-button {
		width: 20px;
		height: 20px;
		border-radius: 0.25rem;
		background-color: #111013;
		border: 1px solid #333137;
		position: relative;
		box-sizing: border-box;
	}

	.checkbox input[type="checkbox"]:checked + .checkbox-button {
		background-color: rgb(0, 29, 194);
		border-color: currentcolor;
	}

</style>