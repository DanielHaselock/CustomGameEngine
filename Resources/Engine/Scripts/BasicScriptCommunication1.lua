local Communication1 =
{
    TestString = "helloWorld",
    StartString
}

function Communication1:OnBegin()
    Debug.Log("Script1 Working : Test String : " .. self.TestString)
    self.StartString = self.TestString

end


function Communication1:OnUpdate(DeltaTime)
    if self.StartString ~= self.TestString then
        Debug.Log("Script 1 Value Changed : NewValue : " .. self.TestString)
        self.StartString = self.TestString
    end
end

return Communication1