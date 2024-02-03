function GetTableDemoBullet()
    local lua_table = {}
    lua_table.GameObject = Scripting.GameObject()
    lua_table.App = Scripting.App()
    lua_table.Transform = Scripting.Transform()
    
    local owner_UID
    local start_time
    local destroy_seconds = 15
    
    function lua_table:Awake()
        owner_UID = lua_table.GameObject:GetMyUID()
    end
    
    function lua_table:Start()
        start_time = lua_table.App:GetGameTime()
    end
    
    function lua_table:Update()  
        current_time = lua_table.App:GetGameTime()
        if (current_time - start_time) > destroy_seconds then
            lua_table.GameObject:DestroyGameObject(owner_UID)
        else
            lua_table.Transform:Translate(owner_UID, 0, 0, 1)
        end
    end
    
    return lua_table
    end