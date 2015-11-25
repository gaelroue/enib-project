#!/usr/bin/lua

-----------------------------------------------------------------------------
htmlprelude=[[
Content-type: text/html

<!DOCTYPE PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
    <head>
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
					<tr>
						<td><pre>
]]

htmlinterlude=[[
					</pre></td>
					</tr>
				</table>
				<table>
					<tr>
						<th>Action result</th>
					</tr>
					<tr>
						<td><pre>
]]

htmlpostlude=[[
						</pre></td>
					</tr>
				</table>
            </div>
        </div>
    </body>
</html>
]]

-----------------------------------------------------------------------------

query=os.getenv("QUERY_STRING")

print(htmlprelude)

-- print the content of the QUERY_STRING env variable
print("QUERYSTRING = [ "..query.." ]")

-- watch for and extract pattern ds7=XXX
s=query:sub(query:find("ds7=%a+"))
led_state=s:sub(5,#s)

print(htmlinterlude)

-- do actions on wanted led state
if led_state=="off" then
	os.execute("echo 0 > /sys/class/leds/ds7/brightness")
	print("led ds7 éteinte à "..os.date())
elseif led_state=="on" then
	os.execute("echo 1 > /sys/class/leds/ds7/brightness")
	print("led ds7 allumée à "..os.date())
end

print(htmlpostlude)
