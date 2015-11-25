#! /usr/bin/lua

-- HTTP header
print [[
Content-Type: text/plain; charset=UTF-8

]]

-- body of page
print "Hello, world\n"

for i = 1, 10 do
  print (i, i * 2)
end
