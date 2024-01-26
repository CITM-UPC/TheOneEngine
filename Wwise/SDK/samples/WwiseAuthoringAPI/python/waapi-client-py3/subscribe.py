#!/usr/bin/env python3
from waapi import WaapiClient, CannotConnectToWaapiException
from pprint import pprint

try:
    # Connecting to Waapi using default URL
    client = WaapiClient()
    # NOTE: the client must be manually disconnected when instantiated in the global scope
except CannotConnectToWaapiException:
    print("Could not connect to Waapi: Is Wwise running and Wwise Authoring API enabled?")
else:
    # Callback function with a matching signature.
    # Signature (*args, **kwargs) matches anything, with results being in kwargs.
    def on_name_changed(*args, **kwargs):
        obj_type = kwargs.get("object", {}).get("type")
        old_name = kwargs.get("oldName")
        new_name = kwargs.get("newName")

        print("Object '{}' (of type '{}') was renamed to '{}'\n".format(old_name, obj_type, new_name))
        client.disconnect()

    handler = client.subscribe("ak.wwise.core.object.nameChanged", on_name_changed, {"return": ["type"]})

    print("Subscribed 'ak.wwise.core.object.nameChanged', rename an object in Wwise")