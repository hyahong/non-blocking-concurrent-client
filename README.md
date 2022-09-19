# non-blocking concurrent client

This is a project to increase download speed over HTTP/HTTPS.

**used OpenSSL for https. so it will work in OpenSSL available environment.**

Splits one large file into multiple pieces, and downloads the pieces at the same time in a `non-blocking`.

<br/>

## Usage

```
$> ./client --help

SYNOPSIS
     ./client [OPTIONS]... [SOURCE]

DESCRIPTION
     -w, --worker WORKER
         specifies the number of workers. default is 10.

     -d, --destination DEST
         specifies the location where the file will be saved.

     -s, --size SIZE
         specifies the size of one file block. default is 20971520 (20 MB).
```

<br/>

## Demo

![gif](https://user-images.githubusercontent.com/12230655/191024988-f4660878-2d04-4087-a2c3-abea0f34f478.gif)

example using 10 workers

<br/>

## Verification
