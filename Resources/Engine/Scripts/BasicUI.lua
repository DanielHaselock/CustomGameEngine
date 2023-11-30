local player =
{
}

function player:OnBegin()
    --self.owner:GetCollider(0):Delegate("OnCollisionEnter")
    self.owner:GetUI(0):DelegateUI(1, "TestFunction", "TestUI")

    

end

function TestFunction()
player.owner:GetUI(0):SetText("TestUI", "HelloWorld") -- cannot use self in this context/ have to use local variable thats the same as OnBegin and OnUpdate
player.owner:GetUI(0):SetScale("TestUI", Vector2.new(0.1, 0.1))
Debug.Log("Lua Hello")

end

return player