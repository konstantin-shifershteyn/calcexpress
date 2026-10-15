function selectorJumpToTargetUrl(selectObj) {
	var targetUrl = selectObj.options[selectObj.selectedIndex].value;

	// Ensure we aren't redirecting to an empty value or the placeholder
	if (!targetUrl || targetUrl === "" || selectObj.selectedIndex === 0) {
		return;
	}

	// Reset the index BEFORE navigating
	selectObj.selectedIndex = 0;

	// Remove Focus: Use .blur() (not window.focus())
	// This is the standard way to stop the scroll wheel from interacting with the element.
	if (typeof selectObj.blur === "function") {
		selectObj.blur();
	}

	// Perform the redirect
	window.location.href = targetUrl;
}
