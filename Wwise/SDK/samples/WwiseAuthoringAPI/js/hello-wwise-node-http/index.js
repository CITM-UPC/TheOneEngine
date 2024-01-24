// Copyright Audiokinetic Inc.

(() => {
    
    const axios = require('axios');
    const ak = require('../../../../include/AK/WwiseAuthoringAPI/js/waapi.js').ak;
    
	const data = {
		uri: ak.wwise.core.getInfo,
		options: {},
		args: {}
	};
	
	const handleResponse = (status, headers, objectPayload) => {
		
		if (status != 200) {
            if (headers["content-type"] == "application/json") {
                console.log(`Error: ${objectPayload.uri}: ${JSON.stringify(objectPayload)}`);
            } else {
                console.log(`Error: ${Buffer.from(objectPayload).toString("utf8")}`);
            }
        } else {
			console.log(`Hello ${objectPayload.displayName} ${objectPayload.version.displayName}`);
		}
	};
	
	axios({
		method: "post",
		url: "http://127.0.0.1:8090/waapi",
		data: data,
		headers: { "content-type": "application/json" }
	}).then((response) => {
		handleResponse(response.status, response.headers, response.data);
	}).catch((err) => {
		if (err.response) {
			handleResponse(err.response.status, err.response.headers, err.response.data);
		} else {
			console.log(`Error: ${err.message}`);
		}
	});
})();
