var divTitle;
var divScroll;

// ============================================
function vih_initFrames() {
	divTitle = document.getElementById("styleDivTitle");
	divScroll = document.getElementById("styleDivScrolling");

	// requestAnimationFrame might not exist in old CHM viewers
	var runner = window.requestAnimationFrame || function(callback) { window.setTimeout(callback, 16); };
	
	runner(function() {
		vih_setFrameHeight();
	});
}

// ============================================
function vih_setFrameHeight()
{
	if (!divTitle || !divScroll) {
		return;
	}
	// we could override divScrollbar settings here, but we preserve settings from CSS
	//	document.body.style.overflowX = "hidden";
	//	document.body.style.overflowY = "hidden";
	//	divTitle.style.overflowX = "hidden";
	//	divTitle.style.overflowY = "hidden";
	//	divScroll.style.overflowX = "hidden";
	//	divScroll.style.overflowY = "scroll";

	// get viewport height (supports older IE/CHM environments)
	var windowHeight = window.innerHeight || document.documentElement.clientHeight || document.body.clientHeight;

	var divTitleHeight = divTitle.offsetHeight;
	var marginTop = 0;
	var marginBottom = 0;

	// Use currentStyle for legacy IE support if getComputedStyle is unavailable
	if (window.getComputedStyle) {
		var style = window.getComputedStyle(divTitle);
		marginTop = parseFloat(style.marginTop) || 0;
		marginBottom = parseFloat(style.marginBottom) || 0;
	} else if (divTitle.currentStyle) {
		// Fallback for very old IE versions
		marginTop = parseFloat(divTitle.currentStyle.marginTop) || 0;
		marginBottom = parseFloat(divTitle.currentStyle.marginBottom) || 0;
	}
    
	divTitleHeight += (marginTop + marginBottom);

	var padding = 5;
	var finalHeight = windowHeight - divTitleHeight - padding;

	if (finalHeight > 0) {
		divScroll.style.height = finalHeight + "px";
	}
	
	vih_resetScrollPosition();
}

// ============================================
function vih_resetScrollPosition() {
	if (!divTitle || !divScroll) {
		return;
	}
	// Guard clause checks our "static" function property
	if (vih_resetScrollPosition.working) {
		return;
	}
//	if (window.scrollY !== 0 || window.scrollX !== 0) {

	// cross-browser scroll detection. Supports old CHM/IE and modern browsers.
	var scrollY = window.pageYOffset || document.documentElement.scrollTop || document.body.scrollTop || 0;
	var scrollX = window.pageXOffset || document.documentElement.scrollLeft || document.body.scrollLeft || 0;

	if (scrollY !== 0 || scrollX !== 0) {
		vih_resetScrollPosition.working = true;
		
		window.scrollTo(0, 0);
		
		// Asynchronously unlock after the browser handles the scroll event
		setTimeout(function() {
			vih_resetScrollPosition.working = false;
		}, 10);
	}
}

// ============================================
// Set Event handlers
if (window.addEventListener) {
	window.addEventListener("load", vih_initFrames);
	window.addEventListener("resize", vih_setFrameHeight);
	window.addEventListener('scroll', vih_resetScrollPosition);
	
} else if (window.attachEvent) {
	// Legacy IE support
	window.attachEvent("onload", vih_initFrames);
	window.attachEvent("onresize", vih_setFrameHeight);
	window.attachEvent("onscroll", vih_resetScrollPosition);
}

// ============================================
