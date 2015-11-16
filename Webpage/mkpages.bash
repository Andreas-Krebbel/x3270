#!/usr/bin/env bash
# Create new-style x3270 webpages, using a common template.
. ./version.txt
if [ "X$1" = "X-g" ]
then	geo_sub="http://x3270.bgp.nu/"
	shift
else	geo_sub=""
fi

unset w3
if [ "X$1" = "X-w" ]
then	w3=1
	shift
fi
# hard wire it now
w3=1

case "$1" in
index)
	title=x3270
	;;
screenshots)
	title="x3270 -- Screenshots"
	;;
documentation)
	title="x3270 -- Documentation"
	;;
documentation-relnotes)
	title="x3270 -- Release Notes"
	;;
documentation-bugs)
	title="x3270 -- Known Bugs"
	;;
documentation-manpages)
	title="x3270 -- Manual Pages"
	;;
documentation-faq)
	title="x3270 -- Frequently Asked Questions"
	;;
documentation-misc)
	title="x3270 -- Miscellaneous Documentation"
	;;
documentation-other)
	title="x3270 -- Other 3270 Resources"
	;;
documentation-ssl)
	title="x3270 -- Secure Connections Using SSL"
	;;
license)
	title="x3270 -- License"
	;;
download)
	title="x3270 -- Download"
	;;
credits)
	title="x3270 -- Credits"
	;;
x026)
	title="x026"
	;;
*)
	echo >&2 "Unknown page name."
	exit 1
	;;
esac
html="$1"
if [ ! -f ${html}-body.html ]
then	echo >&2 "No body file found."
	exit 1
fi

if [ $# -gt 1 ]
then	outfile=$2
else	outfile=${html}.html
fi

(cat <<EOF
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
 <head>
  <title>$title</title>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  <link rel="stylesheet" type="text/css" href="@HOME@styles.css">
 </head>
<body>
 <table width="100%" style="background-color: black;">
  <tbody><tr style="background-color: black;">
    <td style="color: #33cc00; font-size: 48px; padding-left: 20px">x3270</td>
    <td align="right" style="padding-right: 10px; width: 100%">
      <img src="@HOME@x3270.jpg" alt="x3270">
    </td>
  </tr>
 </tbody></table>
 <table width="100%">
  <tbody><tr style="height: 100%">
    <td class=menu style="width: 160px" valign=top>
      <a href="index.html">Home</a><br>
      <a href="@HOME@screenshots.html">Screenshots</a><br>
      <a href="@HOME@documentation.html">Documentation</a><br>
EOF
case "$html" in
documentation*)
	cat <<EOF
      <a href="@HOME@documentation-relnotes.html">&nbsp;&nbsp;Release Notes</a><br>
      <a href="@HOME@documentation-bugs.html">&nbsp;&nbsp;Known Bugs</a><br>
      <a href="@HOME@documentation-manpages.html">&nbsp;&nbsp;Manual Pages</a><br>
      <a href="@HOME@documentation-faq.html">&nbsp;&nbsp;FAQ</a><br>
      <a href="@HOME@documentation-misc.html">&nbsp;&nbsp;Miscellaneous</a><br>
      <a href="@HOME@documentation-other.html">&nbsp;&nbsp;Other Resources</a><br>
EOF
	;;
esac
cat <<EOF
<a href="@HOME@license.html">License</a><br>
      <a href="@HOME@download.html">Download</a><br>
      <a href="@HOME@credits.html">Credits</a><br>
      <p>
        <a href="http://sourceforge.net/projects/x3270"><img src="http://sourceforge.net/sflogo.php?group_id=153338&amp;type=1" width="88" height="31" style="border-width: 0" alt="SourceForge.net Logo" /></a>
      </p>
EOF
if [ "$w3" ]
then	cat <<EOF
      <p>
        <a href="http://validator.w3.org/check?uri=referer"><img
	  src="http://www.w3.org/Icons/valid-xhtml10"
	  alt="Valid XHTML 1.0 Strict" height="31" width="88"
	  style="border-width: 0" /></a>
      </p>
EOF
fi
cat <<EOF
    </td>
    <td class=content valign="top">
EOF
 sed "s/CYEAR/$cyear/g" ${html}-body.html
 cat <<EOF
    </td>
  </tr>
 </tbody></table>
 </body>
</html>
EOF
) | sed "s~@HOME@~${geo_sub}~g" >$outfile
