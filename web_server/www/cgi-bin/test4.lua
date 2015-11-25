#!/usr/bin/lua

dofile "cgiutils.lua"

-- HTTP header
print [[
Content-Type: text/html

]]

-- body of page
-- doctype, page header

print [[
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Atmel Corporation - Software Examples</title>
  <link rel="stylesheet" type="text/css" href="../style.css" />
</head>
<body>
<div id="body">
  <h1>AT91SAM9261-EK development board</h1>
  <div id="banner"></div>
  <div id="contents">
    <table>
      <tr><th>Control ds7 led</th></tr>
      <tr><td>
]]
      
get_data, cookie_data, post_data, request_method = get_user_input ()

if get_data["ds7"]=="on" then
	print [[
        <form  method=get action="test4.lua">
        <input type=radio name="ds7" value="off">off
        <input type=radio name="ds7" value="on" checked>on
        <p></p>
        <hr>
        <input type=submit name="valide" value="valider">
        </form>
	
	]]
else
	print [[
        <form  method=get action="test4.lua">
        <input type=radio name="ds7" value="off" checked>off
        <input type=radio name="ds7" value="on">on
        <p></p>
        <hr>
        <input type=submit name="valide" value="valider">
        </form>
	
	]]
end

print [[
      </td></tr>
    </table>
    <table>
      <tr><th>Action result</th></tr>
]]

-- do actions on wanted led state
if get_data["ds7"]=="on" then
	os.execute("echo 1 > /sys/class/leds/ds7/brightness")
	print("<tr><td><pre>led ds7 allumée à "..os.date().."</pre></td></tr>")
else
	os.execute("echo 0 > /sys/class/leds/ds7/brightness")
	print("<tr><td><pre>led ds7 éteinte à "..os.date().."</pre></td></tr>")
end

-- end of body
print [[
    </table>
  </div>
</div>
</body>
</html>
]]
