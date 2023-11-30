local player =
{
}

function player:OnBegin()
    self.owner:GetParticle(0):SetParticle("helllo.pat")

    self.owner:GetParticle(0):Play()
    Timer.WaitUntil(0.5,"Test")

end


function player:OnUpdate(DeltaTime)


end



function Test()

    player.owner:GetParticle(0):Stop()
end


return player