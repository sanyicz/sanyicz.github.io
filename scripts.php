<?php

//start the session
session_start();
//set session variables
$_SESSION["actualLanguageCode"] = "en"; //set default language

function changeLang($languageCode) {
	$actualLanguageCode = $_SESSION["actualLanguageCode"];
	
	if ($languageCode == "") {
		$languageCode = $actualLanguageCode;
	}
	else if ($languageCode != $actualLanguageCode) {
		//console.log("change language");
	}
	
	$_SESSION["actualLanguageCode"] = $actualLanguageCode;
	
	$all = document.getElementsByTagName("*");
	
	for ($i=1, $max=$all.length; $i < $max; $i++) { //i=0 is the <html lang="en"> element
		$element = $all[$i];
		if ($element.hasAttribute("lang")) {
			//console.log('hasAttribute("lang"):', element);
			$attr = $element.getAttribute("lang");
			if ($attr == $languageCode) {$element.style.display = "";}
			else {$element.style.display = "none";};
		};
	};
	//console.log("done");
	return;
}

changeLang($languageCode);

?>