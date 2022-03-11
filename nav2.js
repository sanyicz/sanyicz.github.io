//load nav bar html
fetch("nav2.html")
.then(result => result.text())
.then(text => {
	let oldElement = document.querySelector("script#menu2");
	let newElement = document.createElement("div");
	newElement.innerHTML = text;
	oldElement.parentNode.replaceChild(newElement, oldElement);
})



//set language
languageCode = localStorage.getItem("actualLanguageCode");
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
