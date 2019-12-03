# virtual-spi-driver
Linux module that implements a fake SPI controller and slave devices, intention is to be able to test userspace SPI code by using this as a loopback

Notes:
* This module isn't actually finished (plans are to switch from the hacky use of miscdevice to setting up a virtual platform device which seems to be the more sane solution)
* Needs to support loopback still
* This is my first attempt at a module so it's really not very polished, I have no idea if a simple module like this would still require some SMP safety
* I'm writing this in a Virtualbox VM under windows for ease, I really don't recommend adding this to a real system
