from time import sleep, strftime
from picamera import PiCamera
camera=PiCamera()
camera.resolution = (1024, 768)
camera.start_preview()
timestr = strftime("%Y.%m.%d-%Hh%M")
camera.capture("images/" + timestr + '.jpg')
camera.close()

