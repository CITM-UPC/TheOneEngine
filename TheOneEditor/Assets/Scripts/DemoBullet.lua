function GetTableDemoBullet()
    local lua_table = {}
    lua_table.GameObject = Scripting.GameObject()
    lua_table.App = Scripting.App()
    
    local owner_UID
    local start_time
    local destroy_seconds = 30
    
    function lua_table:Awake()
        owner_UID = lua_table.GameObject:GetMyUID()
    end
    
    function lua_table:Start()
        start_time = lua_table.App:GetGameTime()
    end
    
    function lua_table:Update()  
        current_time = lua_table.App:GetGameTime()
        if (current_time - start_time) > destroy_seconds then
            -- Destroy the owner GameObject
        end
    end
    
    return lua_table
    end