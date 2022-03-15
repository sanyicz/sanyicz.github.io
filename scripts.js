//set default language as hungarian in first run
var actualLanguageCode = localStorage.getItem("actualLanguageCode");
if (actualLanguageCode == "null") {
	actualLanguageCode = "hu";
	localStorage.setItem("actualLanguageCode", actualLanguageCode);
}
console.log("actualLanguageCode: ", actualLanguageCode);


function changeLang(languageCode) {
	console.log("changeLang running");
	
	actualLanguageCode = localStorage.getItem("actualLanguageCode");
	console.log("1: ", actualLanguageCode);
	
	if (languageCode == "") {
		// console.log("default language");
		//set language as the default one
		languageCode = actualLanguageCode;
	}
	else if (languageCode != actualLanguageCode) {
		//change language
		console.log("change language to", languageCode);
	}
	
	localStorage.setItem("actualLanguageCode", languageCode);
	
	var all = document.getElementsByTagName("*");
	
	for (var i=1, max=all.length; i < max; i++) { //i=0 is the <html lang="en"> element
		element = all[i];
		//console.dir(element);
		if (element.hasAttribute("lang")) {
			//console.log('hasAttribute("lang"):', element);
			var attr = element.getAttribute("lang");
			if (attr == languageCode) {element.style.display = "";}
			else {element.style.display = "none";};
		};
	};
	//console.log("2:", localStorage.getItem("actualLanguageCode"));
	//console.log("done");
	return;
};


function onLoad() {
	//write current year in footer
	document.getElementById("year").innerHTML = new Date().getFullYear();
	//set language
	changeLang("");
};




async function fetchAndInsert(source) {
	//source must be a string
	fetch(source + ".html")
	.then(result => result.text())
	.then(text => {
		var element = document.getElementById(source);
		element.insertAdjacentHTML("afterbegin", text);
		const event1 = new Event(source);
		console.log(source + " fetched");
	})
};

async function onLoad2() {
	let result = await fetchAndInsert();
	console.log("onLoad2 running");
	//write current year in footer
	document.getElementById("year").innerHTML = new Date().getFullYear();
	//set language
	changeLang("");
	return result;
};