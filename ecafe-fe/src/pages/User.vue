<template>
	<main>
		<NavigationBar></NavigationBar>

		<div class="container">
			<div class="container-body">
				<div class="container-header">
					<router-link to='/' class="arrow-left">
						<i class="ri-arrow-left-line"></i>
					</router-link>
					<h1 class="container-title">Create User</h1>
				</div>
				
				<UserForm></UserForm>

				<div class="clients-footer"></div>
			</div>
		</div>

	</main>
</template>

<script>

import NavigationBar from '../components/NavigationBar.vue';
import UserForm from '../components/UserForm.vue';

export default {
	name: "User",
	components: {
		NavigationBar,
		UserForm,
	},
}


</script>