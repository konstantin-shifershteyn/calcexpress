
createObjects();

//////////////////////////////////////////////////////////////////////////////////
// The function creates HtmlHelp ActiveX Objects for targets in LabVIEW Help Files
function createObjects()
{
var arr = [ ""
,	"lvhAPSpect",		"..\\lvanls.chm::/Auto_Power_Spectrum.html"
,	"lvhACorrel",		"..\\lvanls.chm::/AutoCorrelation.html"
,	"lvhConvolution",	"..\\lvanls.chm::/Convolution.html"
,	"lvhCCorrel",		"..\\lvanls.chm::/CrossCorrelation.html"
,	"lvhCorrel",		"..\\lvanls.chm::/Correlation.html"
,	"lvhCPower",		"..\\lvanls.chm::/Cross_Power.html"
,	"lvhCPSpect",		"..\\lvanls.chm::/Cross_Power_Spectrum.html"
,	"lvhDeconvolution",	"..\\lvanls.chm::/Deconvolution.html"
,	"lvhFFT",			"..\\lvanls.chm::/FFT.html"
,	"lvhFHIT",			"..\\lvanls.chm::/Fast_Hilbert_Transform.html"
,	"lvhFHT",			"..\\lvanls.chm::/FHT.html"
,	"lvhIFFT",			"..\\lvanls.chm::/Inverse_FFT.html"
,	"lvhIFHIT",			"..\\lvanls.chm::/I_Fast_Hilbert_Transform.html"
,	"lvhIFHT",			"..\\lvanls.chm::/Inverse_FHT.html"
,	"lvhPSpect",		"..\\lvanls.chm::/Power_Spectrum_Sxx.html"
,	"lvhSpectrum",		"..\\lvanls.chm::/Amp_and_Phase_Spectrum.html"
,	"lvhUnwrapPhase",	"..\\lvanls.chm::/Unwrap_Phase.html"
,	"lvhZeroPad",		"..\\lvanls.chm::/Zero_Padder.html"
,	"lvhGWNoise",		"..\\lvanls.chm::/Gaussian_White_Noise.html"
,	"lvhChirpPtrn",		"..\\lvanls.chm::/Chirp_Pattern.html"
,	"lvhPulsePtrn",		"..\\lvanls.chm::/Pulse_Pattern.html"
,	"lvhPRNoise",		"..\\lvanls.chm::/Periodic_Random_Noise.html"
,	"lvhSincPtrn",		"..\\lvanls.chm::/Sinc_Pattern.html"
,	"lvhSinePtrn",		"..\\lvanls.chm::/Sine_Pattern.html"
,	"lvhSawtWave",		"..\\lvanls.chm::/Sawtooth_Wave.html"
,	"lvhSineWave",		"..\\lvanls.chm::/Sine_Wave.html"
,	"lvhSquareWave",	"..\\lvanls.chm::/Square_Wave.html"
,	"lvhTriangWave",	"..\\lvanls.chm::/Triangle_Wave.html"
,	"lvhUWNoise",		"..\\lvanls.chm::/Uniform_White_Noise.html"
,	"lvhArToSs",		"..\\glang.chm::/Array_To_Spreadsheet_Str.html"
,	"lvhSsToAr",		"..\\glang.chm::/Spreadsheet_Str_To_Array.html"
,	"lvhFmtDt",			"..\\glang.chm::/Format_Date_Time_String.html"
,	"lvhFmtStr",		"..\\glang.chm::/Format_Into_String.html"
,	"lvhScanStr",		"..\\glang.chm::/Scan_From_String.html"
,	"lvhMatchPtrn",		"..\\glang.chm::/Match_Pattern.html"
,	"lvhDerivative",	"..\\gmath.chm::/Derivative_xt.html"
,	"lvhIntegral",		"..\\gmath.chm::/Integral_xt.html"
,	"lvhFlatStr",		"..\\glang.chm::/Flatten_To_String.html"
,	"lvhUnflatStr",		"..\\glang.chm::/Unflatten_From_String.html"
];

root = ceHlpGetCurrentDocFolder();

	document.writeln("<div hidden>");
	for (i=1; i < arr.length; i+=2) {
		document.writeln("<OBJECT id=" + arr[i] + " type='application/x-oleobject' classid='clsid:adb880a6-d8ff-11cf-9377-00aa003b7a11' codebase='hhctrl.ocx'>");
		document.writeln("<PARAM name='Command' value='Related Topics'>");
		document.writeln("<PARAM name='Window' value='$global_lvhelp'>");
		document.writeln("<PARAM name='Item1' value='Item1;" + ceHlpBuildAdsPath(root, arr[i+1]) + "'>");
		document.writeln("</OBJECT>");
	}
	document.writeln("</div>");
}

///////////////////////////////////////////////////////////////
// The function returns a directory path where a HTML file resides
function ceHlpGetCurrentDocFolder()
{
	path = "";
	dir = "";
	url = document.URL;

	arr = url.split(':///');
	if (arr.length >= 2) {
		// ### if document's URL looks like "file:///C:/dir/dir/filename.html"
		path = arr[1];
		arr = path.split('/');
		arr = arr.slice(0, arr.length-1);
		dir = arr.join('\\');
	}
	else {
		arr = url.split('mk:@MSITStore:');
		if (arr.length < 2) {
			arr = url.split('ms-its:');
		}
		if (arr.length >= 2) {
			// ### if document's URL looks like:
			//		"mk:@MSITStore:C:\dir\dir\helpfile.chm::/filename.html"
			// or	"ms-its:C:\dir\dir\helpfile.chm::/filename.html"
			path = arr[1];
			dir = path.split('::/')[0];
			arr = dir.split('\\');
			arr = arr.slice(0, arr.length-1);
			dir = arr.join('\\');
		}
	}
	return dir.replace(/%20/g, " ");	// spaces in a path are represented as '%20'. Replase all '%20' with spaces (use regular expression)
}
///////////////////////////////////////////////////////////////
// The function returns an absolute path, which is built from a root dir path and a relative path
function ceHlpBuildAdsPath(rootDir, relPath)
{
	if (rootDir.substring(rootDir.length-1) == "\\")	rootDir = rootDir.substring(0, rootDir.length()-1);
	if (relPath.substring(0,1) == "\\")					relPath = relPath.substring(1);

	arr = relPath.split("..\\");
	nBack = arr.length - 1;
	_path = arr[nBack];
	//
	arr = rootDir.split('\\');
	_root = arr.slice(0, arr.length - nBack -0).join('\\');

	return _root + "\\" + _path;
}
