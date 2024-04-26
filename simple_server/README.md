# Simple Server Backend

Many, _many_ years ago, when AppEngine and Python 2.7 was king, I set up a simple backend to use for generic applications. It had namespaces, and could store key-value pairs. That's it.

I've found this incredibly useful over the years for various projects, because:

- AppEngine allows HTTP traffic, so simple devices that don't have SSL support can still access it
- Hitting a straightforward HTTP URL is all that's necessary to send data to the server
- It's free

Things I've used it for, and continue to use it for, (un?)fortunately:

- when an ESP goes online, it'll send it's LAN IP address, so I can find it on the network without digging thorugh my router's clumsy interface
- sending simple statistics, like temperature, battery voltage, or GPS coordinates
- sending firmware version, so an ESP can know if a new firwmare is waiting for it somewhere
- sending WAN IP as a sort of dynamic DNS, in case my ISP gives me a new public IP

In any case, since I have code for this the repo, I might as well publish the backend... so here's my :poop: code. Please don't send me updates. This code has been running with ~no modifications for over a decade. (I think I added a "last updated" timestamp about two years ago)

## How to deploy

On App Engine. Using Python 2.7, if they still let (ha)

## Caveats

This version of the code is what I could find at the moment on Google Cloud Source Repositories; I think there's a newer version on some laptop I can't find, but, eh, for these purposes it'll work

Oh and it goes without saying but I'll say it anyway: **DO NOT use this for anything even remotely important.** Or anything not important; use like ThingsBoard or something.

## License

Do what you want, don't sue me, no guarantees of fitness for a particular purpose yadda yadda.