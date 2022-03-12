console.log("Fetching...");

/*var actualLanguageCode = localStorage.getItem("actualLanguageCode");
console.log("actualLanguageCode: ", actualLanguageCode);*/

//load nav bar html
fetch("nav2.html")
.then(result => result.text())
.then(text => {
	let oldElement = document.querySelector("script#menu2");
	let newElement = document.createElement("div");
	newElement.innerHTML = text;
	let attr = document.createAttribute("id");
	attr.value = "menu2";
	newElement.setAttributeNode(attr);
	oldElement.parentNode.replaceChild(newElement, oldElement);
})

console.log("Fetched");
