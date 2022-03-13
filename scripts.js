//set default language as hungarian in first run
var actualLanguageCode = localStorage.getItem("actualLanguageCode");
if (actualLanguageCode == "null") {
	actualLanguageCode = "hu";
	localStorage.setItem("actualLanguageCode", actualLanguageCode);
}
console.log("actualLanguageCode: ", actualLanguageCode);


function changeLang(languageCode) {
	//console.log("running");
	
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
	console.log("onLoad running");
	//write current year in footer
	document.getElementById("year").innerHTML = new Date().getFullYear();
	//set language
	setTimeout(changeLang(""), 300);
	
	/*
	if (localStorage.getItem("actualLanguageCode") == "") {
		//try language setting again in 300 milliseconds
		setTimeout(changeLang(""), 300);
	}
	*/
}