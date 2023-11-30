local player =
{
    canSwitchUi = true
}

function player:OnBegin()
    --self.owner:GetCollider(0):Delegate("OnCollisionEnter")
 --  self.owner:GetUI(0):DelegateUI(1, "TestFunction", "TestUI")

end

function player:OnUpdate(Deltatime)
  --  a = player.owner:GetUI(0):GetVisibility("TestUI")
    if player.canSwitchUi == true then
        if Input.GetKeyPress("P")  then
            -- if a ~= true then
            --     player.owner:GetUI(0):SetVisibility("TestUI", true)
            --     Timer.WaitUntil(2,"Reset")
            --     player.canSwitchUi = false  
            -- end

            Scenes.LoadScene("/Map/TestScene1")
            player.canSwitchUi = false
                 
        end

    end
     
    Timer.WaitUntil(1,"Test")
end

function Reset()

    player.canSwitchUi = true
end


function Test()

    Debug.Log("Working")
end

function TestFunction()
player.owner:GetUI(0):SetText("TestUI", "HelloWorld") -- cannot use self in this context/ have to use local variable thats the same as OnBegin and OnUpdate

a = player.owner:GetUI(0):GetVisibility("TestUI")
if player.canSwitchUi == true then

    if player.owner:GetUI(0):GetVisibility("TestUI") == true then

        player.owner:GetUI(0):SetVisibility("TestUI", false)
    else

        player.owner:GetUI(0):SetVisibility("TestUI", true)

    end
    player.canSwitchUi = false
    Debug.Log("Lua Hello")
    Timer.WaitUntil(2,"Reset")

    end

end

return player