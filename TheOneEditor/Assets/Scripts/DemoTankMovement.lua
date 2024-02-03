function GetTableDemoTankMovement()
local lua_table = {}
lua_table.GameObject = Scripting.GameObject()
lua_table.Input = Scripting.Input()
lua_table.Transform = Scripting.Transform()

local owner_UID
local speed = 1 / 10

function lua_table:Awake()
    owner_UID = lua_table.GameObject:GetMyUID()
end

function lua_table:Start()
end

function lua_table:Update()
    if lua_table.Input:IsKeyRepeat("W") then
        lua_table.Transform:Translate(owner_UID, 0, 0, speed)
    elseif lua_table.Input:IsKeyRepeat("S") then
        lua_table.Transform:Translate(owner_UID, 0, 0, -speed)
    elseif lua_table.Input:IsKeyRepeat("A") then
        lua_table.Transform:Rotate(owner_UID, 0, -1, 0)
    elseif lua_table.Input:IsKeyRepeat("D") then
        lua_table.Transform:Rotate(owner_UID, 0, 1, 0)
    end
end

return lua_table
end