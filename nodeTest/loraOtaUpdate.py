import requests
import sys
from os.path import basename

Import("env")

host = env.GetProjectOption("loraOtaServer")

def publish_firmware2(*args, **kwargs):
    print("Publish Firmware")
    print(kwargs)
    print(args)
    env.Exit(0)
    firmware_path = str(source[0])
    firmware_name = basename(firmware_path)

   

    headers = {
        'Content-Type': 'multipart/form-data'
    }
    
    files = {'file': open(firmware_path, 'rb')}

    r = None
    try:
        r = requests.post(host,
                        #   data=open(firmware_path,"rb")
                        files=files,
                        headers=headers,
                        
                        )
        print(r.json)
    except requests.exceptions.RequestException as e:
        sys.stderr.write("Failed to submit package: %s\n" %
                         ("%s\n%s" % (r.status_code, r.text) if r else str(e)))
        env.Exit(1)

env.AddCustomTarget("loRaOtaUpdate", None, publish_firmware2,"loraOtaUpdate","Lora Ota Update")

def publish_firmware(source,target,env):
    print("Publish Firmware")
    firmware_path = str(source[0])
    firmware_name = basename(firmware_path)

    url = "http://hydro.hydro-babiat.ovh/fileSystem/"

    headers = {
        'Content-Type': 'multipart/form-data'
    }
    
    files = {'filename': open(firmware_path, 'rb')}

    r = None
    try:
        r = requests.post(url,
                        #data=open(firmware_path,"rb"),
                        files=files#,
                        #headers=headers
                        )
        print(r.json)
        #print(r.text)
    except requests.exceptions.RequestException as e:
        sys.stderr.write("Failed to submit package: %s\n" %
                         ("%s\n%s" % (r.status_code, r.text) if r else str(e)))
        env.Exit(1)


def after_build(source,target,env):
    print("AfterBuild")
    print(source[0])
    publish_firmware(source,target,env)

env.AddPostAction("buildprog",after_build)
