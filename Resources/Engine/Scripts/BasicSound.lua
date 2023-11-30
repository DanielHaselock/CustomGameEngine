local soundplayer = 
{
    -- Variables
    elapsedTime = 0,
}

function soundplayer:OnBegin()
    foo = self.owner:GetSound(0)
    foo:SetClip("Ressources/Engine/Sounds/womp-womp.mp3")
end

function soundplayer:OnUpdate(deltaTime)
    self.elapsedTime = self.elapsedTime + deltaTime

    if self.elapsedTime > 1 then
        foo:Play()
        self.elapsedTime = 0
    end
end

return soundplayer