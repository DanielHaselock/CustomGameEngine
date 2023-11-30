local player =
{
    Press = false,
    Speed = 10000,
}

function player:OnBegin()
end


function player:OnUpdate(DeltaTime)
--Tags

Forward = self.owner:GetCamera(0):GetForward()

Right = self.owner:GetCamera(0):GetRight()
Up = self.owner:GetCamera(0):GetUp()

forwardspeed = Forward

forwardspeed.x = forwardspeed.x * player.Speed * DeltaTime
forwardspeed.y = 0
forwardspeed.z = forwardspeed.z * player.Speed * DeltaTime

RightSpeed = Right
RightSpeed.x = RightSpeed.x * player.Speed * DeltaTime
RightSpeed.y = RightSpeed.y * player.Speed * DeltaTime
RightSpeed.z = RightSpeed.z * player.Speed * DeltaTime


UpSpeed = Up
UpSpeed.x = UpSpeed.x * player.Speed * DeltaTime
UpSpeed.y = UpSpeed.y * player.Speed * DeltaTime
UpSpeed.z = UpSpeed.z * player.Speed * DeltaTime


Collider = self.owner:GetCollider(0)
if Collider then
  Debug.Log("Working")
  Collider:ClearLinearVelocity()
  --Collider:ClearForces()

  if Input.GetKeyPress("W") then
  --self.owner:SetTransform(0):SetLocalPosition(Vector3.new(self.owner:SetTransform(0):GetLocalPosition().x + ( forwardspeed.x), self.owner:SetTransform(0):GetLocalPosition().y + (forwardspeed.y), self.owner:SetTransform(0):GetLocalPosition().z + (forwardspeed.z)))
  Collider:SetLinearVelocity(forwardspeed)
  end
  if Input.GetKeyPress("S") then
    --self.owner:SetTransform(0):SetLocalPosition(Vector3.new(self.owner:SetTransform(0):GetLocalPosition().x - (forwardspeed.x),self.owner:SetTransform(0):GetLocalPosition().y - (forwardspeed.y),self.owner:SetTransform(0):GetLocalPosition().z - (forwardspeed.z)))
    Collider:SetLinearVelocity(-forwardspeed)
  end
  if Input.GetKeyPress("A") then
  --self.owner:SetTransform(0):SetLocalPosition(Vector3.new(self.owner:SetTransform(0):GetLocalPosition().x + ( RightSpeed.x),self.owner:SetTransform(0):GetLocalPosition().y + ( RightSpeed.y),self.owner:SetTransform(0):GetLocalPosition().z + ( RightSpeed.z)))
  Collider:SetLinearVelocity(RightSpeed)
  end
  if Input.GetKeyPress("D") then
  --self.owner:SetTransform(0):SetLocalPosition(Vector3.new(self.owner:SetTransform(0):GetLocalPosition().x - ( RightSpeed.x),self.owner:SetTransform(0):GetLocalPosition().y - ( RightSpeed.y),self.owner:SetTransform(0):GetLocalPosition().z - ( RightSpeed.z)))
  Collider:SetLinearVelocity(-RightSpeed)
  end


end
MouseInput =  Input.GetMousePosition()


MouseDifference = Input.GetMouseDifference()



-- Debug.Log("Mouse X ".. MouseDifference.x .. "Mouse Y " .. MouseDifference.y)
-- Debug.Log("Forward X ".. Right.x .. "Forward Y " .. Right.y .. "Forward Z " .. Right.z)

self.owner:GetCamera(0):AddYaw(MouseDifference.y * 0.1)
self.owner:GetCamera(0):AddPitch(-MouseDifference.x * 0.1)



end

return player