var printStyles = [
	'<style media="print" type="text/css">',
	'body {',
	'    overflow: visible !important;',
	'    width: 100% !important;',
	'}',
	'#styleDivTitle {',
	'    background: #FFFFFF !important;',
	'    border-style: none !important;',
	'    overflow: visible !important;',
	'}',
	'#styleDivScrolling {',
	'    overflow: visible !important;',
	'    width: 100% !important;',
	'    height: auto !important;',
	'}',
	'</style>'
].join('\n');

document.write(printStyles);
