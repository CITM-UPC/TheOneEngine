/******************************************************************************

The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2023 Audiokinetic Inc.

*******************************************************************************/

using Newtonsoft.Json.Linq;
using System;
using System.Threading.Tasks;

namespace AK.Wwise.Waapi
{
    class Program
    {
        static void Main(string[] args)
        {
            _Main().Wait();
        }

        static async Task _Main()
        {
            try
            {
                AK.Wwise.Waapi.JsonClient client = new AK.Wwise.Waapi.JsonClient();

                // Try to connect to running instance of Wwise on localhost, default port
                await client.Connect();

                // Register for connection lost event
                client.Disconnected += () =>
                {
                    System.Console.WriteLine("We lost connection!");
                };

                // Simple RPC call
                JObject info = await client.Call(ak.wwise.core.getInfo, null, null);
                System.Console.WriteLine(info);

                // Create an object for our tests, using C# anonymous types
                var testObj = await client.Call(
                    ak.wwise.core.@object.create,
                    new
                    {
                        name = "WaapiObject",
                        parent = @"\Actor-Mixer Hierarchy\Default Work Unit",
                        type = "ActorMixer",
                        onNameConflict = "rename"
                    }, null);

                System.Console.WriteLine(testObj["id"]);

                // Subscribe to name changes
                int nameSubscriptionId = await client.Subscribe(
                    ak.wwise.core.@object.nameChanged,
                    null,
                    (JObject publication) =>
                    {
                        System.Console.WriteLine($"Name changed: {publication}");
                    });

                // Subscribe to property changes
                int propertySubscriptionId = await client.Subscribe(
                    ak.wwise.core.@object.propertyChanged,
                    new JObject(
                        new JProperty("property", "Volume"),
                        new JProperty("object", testObj["id"])),
                    (JObject publication) =>
                    {
                        System.Console.WriteLine($"Property '{publication["property"]}' changed: {publication["new"]}");
                    });


                // Set name using C# anonymous types
                try
                {
                    await client.Call(
                        ak.wwise.core.@object.setName,
                        new
                        {
                            @object = testObj["id"],
                            value = "NewName"
    
                        }, null);

                    // Undo the set name, using JObject constructor
                    await client.Call(
                        ak.wwise.ui.commands.execute,
                        new JObject(new JProperty("command", "Undo")), null);
                }
                catch (AK.Wwise.Waapi.Wamp.ErrorException e)
                {
                    System.Console.Write(e.Message);
                    // Ignore the error, it is possible we have a name conflict
                }

                // Set property using anonymous type
                await client.Call(
                    ak.wwise.core.@object.setProperty,
                    new
                    {
                        @property = "Volume",
                        @object = testObj["id"],
                        value = 6
                    }, null);

                // Setting a property using dynamic types
                {
                    dynamic args = new JObject();
                    args.property = "Volume";
                    args.@object = testObj["id"];
                    args.value = 9;

                    // Set property with JObject
                    await client.Call(ak.wwise.core.@object.setProperty, args, null);
                }

                // Setting a property using JObject constructor
                await client.Call(ak.wwise.core.@object.setProperty,
                    new JObject(
                        new JProperty("property", "Volume"),
                        new JProperty("object", testObj["id"]),
                        new JProperty("value", 10)),
                    null);

                {
                    // Query the volume, using JObject constructor
                    var query = new JObject(
                        new JProperty("from", new JObject(
                            new JProperty("id", new JArray(new string[] { testObj["id"].ToString() })))));

                    var options = new JObject(
                        new JProperty("return", new string[] { "name", "id", "@Volume" }));

                    JObject result = await client.Call(ak.wwise.core.@object.get, query, options);
                    System.Console.WriteLine(result["return"][0]["@Volume"]);
                }

                {
                    // Query the project using anonymous objects
                    var query = new
                    {
                        from = new
                        {
                            id = new string[] { testObj["id"].ToString() }
                        }
                    };
                    var options = new
                    {
                        @return = new string[] { "name", "id", "@Volume", "path" }
                    };

                    JObject jresult = await client.Call(ak.wwise.core.@object.get, query, options);
                    System.Console.WriteLine(jresult["return"][0]["@Volume"]);
                }

                // Clean up the created objects!
                await client.Call(
                    ak.wwise.core.@object.delete,
                    new JObject(new JProperty("object", testObj["id"])), null);

                await client.Unsubscribe(nameSubscriptionId);
                await client.Unsubscribe(propertySubscriptionId);

                await client.Close();

                System.Console.WriteLine("done");
            }
            catch (Exception e)
            {
                System.Console.Error.WriteLine(e.Message);
            }
        }
    }
}
