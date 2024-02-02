function GetTableDemoTurretMovement()
local lua_table = {}
lua_table.GameObject = Scripting.GameObject()
lua_table.Input = Scripting.Input()
lua_table.Transform = Scripting.Transform()

local owner_UID

function lua_table:Awake()
    owner_UID = lua_table.GameObject:GetMyUID()
end

function lua_table:Start()
end

function lua_table:Update()
    local x_movement = lua_table.Input:GetGetMouseMovementX()
    -- FIXME: The optimal way to do this would be to grab screen size and divide the movement
    -- and multiply it by sensitivity, but this will do
    if (x_movement > 0) then
        lua_table.Transform:Rotate(owner_UID, 0, 1, 0)
    else if (x_movement < 0) then
        lua_table.Transform:Rotate(owner_UID, 0, 1, 0)
    end

    -- TODO: Mouse clicking should instantiate a new bullet?

end

return lua_table
end