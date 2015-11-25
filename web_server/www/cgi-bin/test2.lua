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
      <tr>
        <th>Get QUERY_STRING env variable setup by web server (GET method)</th>
      </tr>
      <tr><td><pre>
]]

-- print the content of the QUERY_STRING env variable
query=os.getenv("QUERY_STRING")
if query then
  print("QUERYSTRING = [ "..query.." ]")
end

print [[
      </pre></td></tr>
      </table>
      <table>
      <tr><th>Action result</th></tr>
      <tr><td>
]]

get_data, cookie_data, post_data, request_method = get_user_input ()

-- do actions on wanted led state
if get_data["ds7"]=="on" then
	os.execute("echo 1 > /sys/class/leds/ds7/brightness")
	print("led ds7 allumée à "..os.date())
elseif get_data["ds7"]=="off" then
	os.execute("echo 0 > /sys/class/leds/ds7/brightness")
	print("led ds7 éteinte à "..os.date())
end

--[[print('<pre>')
for k,v in pairs(get_data) do
  print(k,v)
end
print('</pre>')]]

print "<h2>GET data</h2>\n"
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
print('</pre>')

-- end of body

print [[
     </td></tr>
      </table>
    </div>
  </div>
</body>
</html>
]]
