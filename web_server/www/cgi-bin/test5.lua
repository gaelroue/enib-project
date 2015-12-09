#!/usr/bin/lua

dofile "cgiutils.lua"

-- HTTP header
print [[
Content-Type: text/html

]]

get_data, cookie_data, post_data, request_method = get_user_input ()

print("<table>")
-- do actions on wanted led state
if get_data["ds7"]=="true" then
	os.execute("echo 1 > /sys/class/leds/ds7/brightness")
	print("<tr><td><pre>led ds7 allumée à "..os.date().."</pre></td></tr>")
else
	os.execute("echo 0 > /sys/class/leds/ds7/brightness")
	print("<tr><td><pre>led ds7 éteinte à "..os.date().."</pre></td></tr>")
end

-- debug information
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
print('</pre></td></tr></table>')
