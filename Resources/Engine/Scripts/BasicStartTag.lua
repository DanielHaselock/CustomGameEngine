local player =
{
}

function player:OnBegin()
    self.owner:SetTag("Block")

end


function player:OnUpdate(DeltaTime)
end

return player