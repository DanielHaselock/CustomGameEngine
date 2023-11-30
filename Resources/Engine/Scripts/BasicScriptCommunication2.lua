local Communication2 =
{
    TestString2 = "helloWorld2",
    OtherScript = nil,
    CurrentScene = nil,
    Timer = 0,
}

function Communication2:OnBegin()


end


function Communication2:OnUpdate(DeltaTime)
  --  Debug.Log("Script2 Working")

    self.Timer = self.Timer + DeltaTime
    if math.floor(self.Timer) == 3 then
        OtherActors = Scenes.GetCurrentScene():GetActorsByTag("Object1")
        for element in elementIterator(OtherActors) 
        do
            self.OtherScript = element:GetScript("BasicScriptCommunication1")
            Debug.Log("Script2 communicating to Script1 : Script1Value :  " .. self.OtherScript.TestString)

            self.OtherScript.TestString = "NopeThisIsChanged"
            Debug.Log("Script2 Changing Script1's Value : new value : " .. self.OtherScript.TestString)
            self.Timer = 0
        end
    end
end

function elementIterator (collection)

    local index = 0
    local count = #collection
     
    -- The closure function is returned
     
    return function ()
       index = index + 1
         
       if index <= count
       then
          -- return the current element of the iterator
          return collection[index]
       end
         
    end
     
end

return Communication2