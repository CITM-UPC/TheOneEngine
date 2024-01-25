
# C# Samples
## Overview

The Visual Studio solution provided in this directory targets .NET 4.5, and includes the following modules:
- **WaapiClientCore**: A Waapi client library implemented in C# solely depending on .NET 4.5, with no 3rd party reference. It includes a WAMP client that is able to connect to Wwise and access WAAPI remote procedures and topics. The API is string-based, so you might need to provide your own JSON serialization. This client is also compatible with Unity.
- **WwiseClientJson**: A waapi client library implemented in C# with a dependency on [Json.NET](https://www.newtonsoft.com/json). The Json dependency is added as a convenience to manipulate JSON. NuGet package manager extension is required. This client can be compatible with Unity if you also install `Json.NET for Unity` from Unity Store.
- **WwiseClientSample**: A C# console application demonstrating the usage of the C# WwiseClientJson library.

## Requirements

By default, Wwise Authoring API should be activated and properly set up. This sample assumes the following:

- In Wwise, select **Project > User Preferences**. (Default shortcut: Shift + U)
- In the Wwise Authoring API group box, select **Enable Wwise Authoring API**.
- Make sure you **Allow connections from** `127.0.0.1,::1`, and the **WAMP port** is set to `8080`.
- Click **OK**.

Visual Studio with C# support and the NuGet package manager extension is required.

## WwiseClientSample Setup
1. Open the current directory in a command line (`<Wwise>/SDK/samples/WwiseAuthoringAPI/cs`)
2. Open the solution `WaapiClientCs_vc160.sln`, right-click on the solution and select "Restore NuGet Packages".
This will download the necessary dependencies such as Json.NET.
3. Close and reopen the solution to update the references.
4. Build the project and observe that there are no errors in the main projects.

## Execution

To run the sample, run WwiseClientSample.
