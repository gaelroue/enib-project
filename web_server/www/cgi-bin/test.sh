#! /bin/sh

## HTTP header
echo "Content-Type: text/plain; charset=UTF-8"
echo "Set-Cookie: foo=bar"
echo "Set-Cookie: wonder=always"
echo

## body of page: get env vars
set
