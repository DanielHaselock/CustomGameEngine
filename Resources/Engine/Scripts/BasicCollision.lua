local player =
{
}

function player:OnBegin()
    self.owner:SetTag("Block")
    self.owner:GetCollider(0):Delegate("OnCollisionEnter")
    self.owner:GetCollider(0):Delegate("OnCollisionExit")
    self.owner:GetCollider(0):Delegate("OnTriggerExit")
    self.owner:GetCollider(0):Delegate("OnTriggerEnter")
   -- self.owner:GetCollider(0):Delegate("e", "continous", "MyFunction")
    GetKey("e")
    Debug.Log("Working")
end


function player:OnUpdate(DeltaTime)
end

function OnCollisionEnter(OtherActor)
    Debug.Log("Collision From Lua")
end

function OnTriggerEnter(OtherActor)
    Debug.Log("Triggering From Lua")
end

function OnCollisionStay(OtherActor)
    Debug.Log("Staying Collision From Lua")
end

function OnTriggerStay(OtherActor)
    Debug.Log("Staying Trigger From Lua")
end

function OnTriggerExit(OtherActor)
    Debug.Log("Exiting Trigger")
end

function OnCollisionExit(OtherActor)
    Debug.Log("Exiting Collision")
end

return player