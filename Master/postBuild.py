import requests
import sys
from os.path import basename

Import('env')

def publish_firmware(source,target,env):
    print("Publish Firmware")
    firmware_path = str(source[0])
    firmware_name = basename(firmware_path)

    url = "http://hydro.hydro-babiat.ovh/fileSystem"

    r = None
    try:
        r = requests.post(url,
                          data=open(firmware_path,"rb"))
    except requests.exceptions.RequestException as e:
        sys.stderr.write("Failed to submit package: %s\n" %
                         ("%s\n%s" % (r.status_code, r.text) if r else str(e)))
        env.Exit(1)

def after_build(source,target,env):
    print("AfterBuild")
    print(source[0])
    publish_firmware(source,target,env)

env.AddPostAction("buildprog",after_build)