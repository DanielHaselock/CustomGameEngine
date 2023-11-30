local player =
{
    Animator
}

function player:OnBegin()
    
    player.Animator = self.owner:GetAnimator(0)
    player.Animator:SetSecondAnimation("MM_Run_Fwd.fbx")
    player.Animator:SetShouldCrossfade(false)
    player.Animator:Play()
    Timer.WaitUntil(2,"StartBlend")
end


function player:OnUpdate(DeltaTime)

end

function StartBlend()
    player.Animator:SetCrossfadeTime(10)
    player.Animator:SetShouldCrossfade(true)
end

-- function StopAnim()
--     player.Animator:Stop()

-- end

return player