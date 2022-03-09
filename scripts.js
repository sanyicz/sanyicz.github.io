/*if (localStorage.getItem("actualLanguageCode") == null) {
	localStorage.setItem("actualLanguageCode", "hu");
}*/
/*
var actualLanguageCode;
if (localStorage.getItem("actualLanguageCode") == null) {
	actualLanguageCode = "hu";
}
else {
	actualLanguageCode = localStorage.getItem("actualLanguageCode");
}*/
/*var actualLanguageCode = localStorage.getItem("actualLanguageCode");
console.log("actualLanguageCode: ", actualLanguageCode);*/


function changeLang(languageCode) {
	//set default language as hun in first run
	if (localStorage.getItem("actualLanguageCode") == null) {
		localStorage.setItem("actualLanguageCode", "hu");
	}
	
	//console.log("running");
	
	//write current year in footer
	document.getElementById("year").innerHTML = new Date().getFullYear();
	
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
