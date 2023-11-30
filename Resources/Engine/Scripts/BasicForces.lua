local player =
{
}

function player:OnBegin()
end


function player:OnUpdate(DeltaTime)
    self.owner:GetCollider(0):AddForce(Vector3.new(100000000,0,0));
    Debug.Log("Working")
end


return player