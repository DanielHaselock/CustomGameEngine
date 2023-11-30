local player =
{
    Press = false,
}

function player:OnBegin()
end


function player:OnUpdate(DeltaTime)
    --Tags

   if Input.GetKeyPress("W") then
   self.owner:SetTransform(0):SetLocalPosition(Vector3.new(self.owner:SetTransform(0):GetLocalPosition().x,self.owner:SetTransform(0):GetLocalPosition().y,self.owner:SetTransform(0):GetLocalPosition().z + 50 * DeltaTime))
  elseif Input.GetKeyPress("S") then
  self.owner:SetTransform(0):SetLocalPosition(Vector3.new(self.owner:SetTransform(0):GetLocalPosition().x,self.owner:SetTransform(0):GetLocalPosition().y,self.owner:SetTransform(0):GetLocalPosition().z - 50 * DeltaTime))
  elseif Input.GetKeyPress("A") then
  self.owner:SetTransform(0):SetLocalPosition(Vector3.new(self.owner:SetTransform(0):GetLocalPosition().x + 50 * DeltaTime,self.owner:SetTransform(0):GetLocalPosition().y,self.owner:SetTransform(0):GetLocalPosition().z))
  elseif Input.GetKeyPress("D") then
  self.owner:SetTransform(0):SetLocalPosition(Vector3.new(self.owner:SetTransform(0):GetLocalPosition().x - 50 * DeltaTime,self.owner:SetTransform(0):GetLocalPosition().y,self.owner:SetTransform(0):GetLocalPosition().z))
  end

   Debug.Log("Working")
end

return player