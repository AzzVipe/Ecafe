import axios from 'axios';

const actions = [
	{
		name: "Lock",
		cmd: "lock",
		onRequest: null,
		onResponse: null,
		overrideResponseHandler: false,
		canSendRequest: true,
		canShowToast: true,
		neadRefresh: true,
		massActionable: true,
	},
	{
		name: "Unlock",
		cmd: "unlock",
		overrideResponseHandler: false,
		canSendRequest: true,
		onRequest: null,
		onResponse: null,
		canShowToast: true,
		neadRefresh: true,
		massActionable: true,
	},
	{
		name: "Ping",
		cmd: "ping",
		onRequest: null,
		onResponse: null,
		canSendRequest: true,
		overrideResponseHandler: false,
		canShowToast: true,
		neadRefresh: false,
		massActionable: true,
	},
	{
		name: "Poweroff",
		cmd: "poweroff",
		onRequest: null,
		onResponse: null,
		canSendRequest: true,
		overrideResponseHandler: false,
		canShowToast: true,
		neadRefresh: true,
		massActionable: true,
	},
	{
		name: "Message",
		cmd: "message",
		onRequest: (action, data) => {
			let msg = prompt("Write your message");
			data.append("msg", msg);
			return sendRequest(action, data);
		},
		onResponse: null,
		canSendRequest: false,
		overrideResponseHandler: false,
		canShowToast: true,
		neadRefresh: false,
		massActionable: false,
	},
	{
		name: "Screenshot",
		cmd: "screenshot",
		onRequest: null,
		onResponse: null,
		canSendRequest: true,
		overrideResponseHandler: true,
		canShowToast: false,
		neadRefresh: false,
		massActionable: false,
	},
];

function sendRequest(action, data) {
	let url = `/client/${action.cmd}`;
	url = `${url}?${data.toString()}`;

	return new Promise((resolve, reject) => {
		axios.get(url).then((response) => {

			if (action.onResponse) {
				action.onResponse(action, response);
			}
			resolve(response);
			// this.toastStatus = true;
			// if (response.data.message) {
			// 	this.msg = response.data.message;
			// }
		}).catch((error) => {
			console.error(error);
			reject(error);
			// this.toastStatus = false;
			// @TODO
			// this.msg = error.data.message;
		}).finally(() => {
			// this.action = "0";
			// this.isPerformingAction = false;
			// this.showToast = true;
			// setTimeout(() => {
			// 	this.showToast = false;
			// 	this.msg = "";
			// }, 4000);
		});
	})

}

function performAction(action, data) {

	if (action.onRequest && action.canSendRequest === false) {
		return action.onRequest(action, data);
	} else if (action.onRequest) {
		action.onRequest(action, data);
	}
	if (action.canSendRequest) {
		return sendRequest(action, data);
	}
}

export { actions, sendRequest, performAction };