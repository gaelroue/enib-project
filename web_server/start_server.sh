#!/bin/sh

echo "______________________________________________"
echo "_________________Start server_________________"
echo "______________________________________________"
echo ""
sudo ps -ef | grep lighttpd | grep -v grep | awk '{print $2}' | xargs kill

echo ""
echo "___________________LightTPD___________________"
echo ""
sudo lighttpd -f /home/sotchi/Documents/ENIB/S9/enib_project/web_server/lighttpd.conf
echo ""
echo "__________________Websocket___________________"
echo ""
#sudo /home/vm/Bureau/PRE/webserver/websocket/websocket_test

