# non-blocking concurrent client

This is a project to increase download speed over HTTP/HTTPS.

**used OpenSSL for https. so it will work in OpenSSL available environment.**

Splits one large file into multiple pieces, and downloads the pieces at the same time in a `non-blocking`.

(This project was tested in `Ubuntu 20.04.4 LTS` environment.)

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

<br/>

## Verification

`md5sum` was used to verify that the target file was downloaded properly.

<br/>

`Original File (in Windows)`
```
$> Get-FileHash -Path .\debian-11.4.0-amd64-netinst.iso -Algorithm md5

Algorithm       Hash                                                                   Path
---------       ----                                                                   ----
MD5             D78B390D70E4A858B41D6BDFDD4B27A0                                       E:\debian-11.4.0...
```

`Downloaded File (in Ubuntu)`
```
$> md5sum debian-11.4.0-amd64-netinst.iso 

d78b390d70e4a858b41d6bdfdd4b27a0  debian-11.4.0-amd64-netinst.iso
```

<br/>

You can see that the `md5sum` of the original and downloaded files are the same.

