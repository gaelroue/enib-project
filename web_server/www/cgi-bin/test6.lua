#!/usr/bin/lua

dofile "cgiutils.lua"

-- HTTP header
print [[
Content-Type: text/html

]]

get_data, cookie_data, post_data, request_method = get_user_input ()

-- do actions on wanted led state
if get_data["ds7"]=="true" then
	os.execute("echo 1 > /sys/class/leds/ds7/brightness")
	print(" Led ds7 allumée le "..os.date())
else
	os.execute("echo 0 > /sys/class/leds/ds7/brightness")
	print(" Led ds7 éteinte le "..os.date())
end
