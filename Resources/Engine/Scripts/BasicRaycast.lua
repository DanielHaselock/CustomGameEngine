local player =
{
    elapsed = 0,
    Timer = 0,
--tests
}
local RaycastCallback = RaycastCallback:new()

function player:OnBegin()
    Debug.Log("Start")

end


function player:OnUpdate(DeltaTime)
    self.elapsed = self.elapsed + DeltaTime
    --self.owner:GetTransform(0):SetLocalPosition(Vector3.new(math.sin(self.elapsed), math.sin(self.elapsed),math.sin(self.elapsed)))
    --self.owner:GetTransform(0):SetLocalRotation(Quaternion.new(0, math.sin(self.elapsed)* 3.1415,0, 1))
   -- Debug.Log("Elapsed time :" .. self.elapsed)
    --Model

    -- if self.owner:GetModel(0) then
    --     if math.floor(self.elapsed) % 3 == 0 then
    --         self.owner:GetModel(0):SetModel("C:/Users/d.haselock/Documents/GP2/Moteur/CoorsLight/Resources/Engine/Models/Torus.fbx")
    --     elseif math.floor(self.elapsed) % 3 == 1 then
    --         self.owner:GetModel(0):SetModel("C:/Users/d.haselock/Documents/GP2/Moteur/CoorsLight/Resources/Engine/Models/Sphere.fbx")
    --     else
    --         self.owner:GetModel(0):SetModel("C:/Users/d.haselock/Documents/GP2/Moteur/CoorsLight/Resources/Engine/Models/Pyramid.fbx")
    --     end  
    -- end

    --Raycast
    Forward = Vector3.new(self.owner:GetTransform(0):GetLocalForward().x, self.owner:GetTransform(0):GetLocalForward().y, self.owner:GetTransform(0):GetLocalForward().z) 
    Forward.x = Forward.x * 100
    Forward.y = Forward.y * 100
    Forward.z = Forward.z * 100

    EndPos = Vector3.new(Forward + self.owner:GetTransform(0):GetLocalPosition())

    --Debug.Log("Forward :" .. EndPos.x .. "   " .. EndPos.y .. "  " .. EndPos.z)
    self.Timer = self.Timer + DeltaTime
  -- Debug.Log("Timer :" .. self.Timer)
    if math.floor(self.Timer) == 3 then
        RayCastCallback = Physics.Raycast(self.owner:GetTransform(0):GetLocalPosition(), EndPos, 1, -1)
        GameObject = Physics.GetActor(RayCastCallback.CollisionObject)
            if GameObject then
            Tag = GameObject:GetTag(0)
            Debug.Log("other Actor Tag :" .. Tag)
            Debug.Log("other Actor Transfrom :" .. GameObject:GetTransform(0):GetLocalPosition().x)
            --GameObject:SetTransform(0):SetLocalPosition(self.owner:GetTransform(0):GetLocalPosition())
            Debug.Log("Collision Point :" .. RayCastCallback.CollisionPoint.x .. " " .. RayCastCallback.CollisionPoint.y .. " "  .. RayCastCallback.CollisionPoint.z)
            Debug.Log("Collision Normal :" .. RayCastCallback.CollisionNormal.x .. " " .. RayCastCallback.CollisionNormal.y .. " "  .. RayCastCallback.CollisionNormal.z)
            self.Timer = 0
        end
    end

end

return player