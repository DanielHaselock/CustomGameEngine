local player =
{
}

function player:OnBegin()
Timer.WaitUntil(5,"loop")
Debug.Log("Working")
end

function player:OnUpdate(DeltaTime)

end

function loop()

    Debug.Log("Hello")
end

return player