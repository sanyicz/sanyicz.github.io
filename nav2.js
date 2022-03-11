fetch("nav2.html")
.then(result => result.text())
.then(text => {
	let oldElement = document.querySelector("script#hereNav2");
	let newElement = document.createElement("div");
	newElement.innerHTML = text;
	oldElement.parentNode.replaceChild(newElement, oldElement);
})