<template>
	<form action="">
		<div class="form-container">
			<div 
				v-for="item in inputForm" 
				:class="[item.isAddressField ? 'address-field' : 'generic-field' ]" 
				:key="item.id">

				<label :for= "item.fname">{{item.label}}</label>
				<textarea 
					:id= "item.fname" 
					:name= "item.fname" 
					:placeholder="item.placeholder" 
					v-if="item.isAddressField" 
					v-model.lazy="item.value" >
					
				</textarea>
				<input 
					:id= "item.fname" 
					:name= "item.fname" 
					:placeholder="item.placeholder" 
					v-model.lazy="item.value"
					v-else>
			</div>
		</div>
	</form>
</template>

<script>

export default {
	name: "UserForm",

	data() {
		return {
			inputForm : [
				{
					id: 1,
					name: "fname",	
					label: "First name",	
					placeholder: "First name",	
					value: "",
					isAddressField: false,
				},
				{
					id: 2,
					name: "lname",	
					label: "Last name",
					placeholder: "Last name",		
					value: "",
					isAddressField: false,
				},
				{
					id: 3,
					name: "address",	
					label: "Address",
					placeholder: " ",		
					value: "",
					isAddressField: true,
				},
				{
					id: 4,
					name: "email",	
					label: "Email ID",	
					placeholder: "abc@example.com",	
					value: "",
					isAddressField: false,
				},
				{
					id: 5,
					name: "contact",	
					label: "Contact number",
					placeholder: "+ 91",		
					value: "",
					isAddressField: false,
				},
				{
					id: 6,
					name: "aadhar",	
					label: "Aadhar number",	
					placeholder: "XXXXXXXXXX",	
					value: "",
					isAddressField: false,
				},
			]

		}
	},
}

</script>