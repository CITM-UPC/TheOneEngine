function GetTableDemoTurretMovement()
local lua_table = {}
lua_table.GameObject = Scripting.GameObject()
lua_table.Input = Scripting.Input()
lua_table.Transform = Scripting.Transform()

local owner_UID
local bullet_UID

function lua_table:Awake()
    owner_UID = lua_table.GameObject:GetMyUID()
    bullet_UID = lua_table.GameObject:FindGOByName("InstanceBullet")
end

function lua_table:Start()
end

function lua_table:Update()
    local x_movement = lua_table.Input:GetMouseMovementX()
    -- FIXME: The optimal way to do this would be to grab screen size and divide the movement
    -- and multiply it by sensitivity, but this will do
    if x_movement > 0 then
        lua_table.Transform:Rotate(owner_UID, 0, -1, 0)
    elseif x_movement < 0 then
        lua_table.Transform:Rotate(owner_UID, 0, 1, 0)
    end

    -- TODO: Mouse clicking should instantiate a new bullet?
    if lua_table.Input:IsMBDown("left") then
        Shoot()
    end
end

function Shoot()
    -- We get our coordinates and orientation
    local my_position = lua_table.Transform:GetGlobalPosition(owner_UID)
    local my_rotation = lua_table.Transform:GetGlobalRotation(owner_UID)

    -- We instance the bullet and set its position and rotation
    local new_bullet_UID = lua_table.GameObject:InstantiateGameObject(bullet_UID)
    lua_table.Transform:SetPosition(new_bullet_UID, my_position["x"], my_position["y"], my_position["z"])
    lua_table.Transform:Translate(new_bullet_UID, 0, 0, 1) -- Move to the tip
    lua_table.Transform:SetRotation(new_bullet_UID, my_rotation["w"], my_rotation["x"], my_rotation["y"], my_rotation["z"])
end

return lua_table
end