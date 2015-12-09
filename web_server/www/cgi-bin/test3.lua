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
        <form  method=get action="test3.lua">
        <input type=radio name="ds7" value="off">off
        <input type=radio name="ds7" value="on" checked>on
        <p></p>
        <hr>
        <input type=submit name="valide" value="valider">
        </form>
	
	]]
else
	print [[
        <form  method=get action="test3.lua">
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
    <tr>
      <th>Get QUERY_STRING env variable setup by web server (GET method)</th>
    </tr>
]]

-- print the content of the QUERY_STRING env variable
query=os.getenv("QUERY_STRING")
if query then
  print("<tr><td><pre>QUERYSTRING = [ "..query.." ]</pre></td></tr>")
end

print [[
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

print "<tr><td><h2>GET data</h2>\n"
show_table (get_data)

print "<h2>COOKIE data</h2>\n"
show_table (cookie_data)

print "<h2>POST data</h2>\n"
show_table (post_data)

print ("<h2>Request method = ", request_method, "</h2>")

print "<h2>ENV VARS</h2>\n"
print('<pre>')
fname = os.tmpname ()
os.execute ("/bin/sh -c set > " .. fname)
f = io.open (fname, "r")  -- open it
s = f:read ("*a")  -- read all of it
print (s)
f:close ()  -- close it
os.remove (fname)
print('</pre></td></tr>')

-- end of body
print [[
    </table>
  </div>
</div>
</body>
</html>
]]
