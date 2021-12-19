<template>
	<div class="clients-header">
		<div class="clients-header-container">
			<!-- <i class="ri-shut-down-line clients-header-icon" @click="$emit('on-shutdown')"></i>  -->
			<label class="search-box">
				<i class="ri-search-2-line"></i>
				<input @input="$emit('update:modelValue', $event.target.value)" class="input" type="text" name="q" placeholder="Search client..." autocomplete="off">
			</label>
		</div>
	</div>
</template>

<script>
export default {
	name: "ClientHeader",
	emits: ['update:modelValue'],
	props: ['modelValue'],
}
</script>