# Waapi-Client Python Samples
## Overview
Samples in this directory demonstrate the usage of the [waapi-client](https://github.com/audiokinetic/waapi-client-python) library for Python 3. This library provides a decoupled autobahn WAMP client with support for plain options and bindable subscription callbacks.

## Requirements
* Python 3.7+
* Wwise instance with the Wwise Authoring API enabled (`Project > User Preferences... > Enable Wwise Authoring API`)

The waapi-client library for Python 3 is available on the [Python Package Index](https://pypi.org/project/waapi-client/) (PyPi) and can be installed with the pip package manager.

* Windows: `py -3 -m pip install waapi-client`
* Other platforms: `python3 -m pip install waapi-client`

## Execution
### Call a remote procedure (rpc.py)
With a project open in Wwise, run the script in a terminal and observe the output. You should see something like:

```
Getting Wwise instance information:
[Wwise Information Dictionary dump]

Query the Default Work Unit information:
{'return': [{'id': '{CDF62889-48AA-436C-B7DD-5B6B1DF5050D}',
             'name': 'Default Work Unit',
             'type': 'WorkUnit'}]}
```

### Subscribe to topic (subscribe.py)
With a project open in Wwise, run the script in a terminal and observe the output. You should see something like:

```
Subscribed 'ak.wwise.core.object.nameChanged', rename an object in Wwise
```

Proceed to rename an object in Wwise. You should then observe something like:

```
Object 'MySound' (of type 'Sound') was renamed to 'MyOtherSound'
```
