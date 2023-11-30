local player =
{
}

function player:OnBegin()
    

end


function player:OnUpdate(DeltaTime)
    --Tags
    self.owner:SetTag("FirstTag")
    Debug.Log("working 1 Tag : " .. self.owner:GetTag(0) .. " Tag Count : " .. self.owner:GetTagCount())
    self.owner:SetTag("SecondTag")
    Debug.Log("working 2 Tag : " ..  self.owner:GetTag(0) .. " " .. self.owner:GetTag(1) .. " Tag Count : " .. self.owner:GetTagCount())
    self.owner:ChangeTag("FirstTag", "ThirdTag")
    Debug.Log("working 3 Tag : " ..  self.owner:GetTag(0) .. " Tag Count : " .. self.owner:GetTagCount())
    self.owner:DeleteTag("SecondTag")
    Debug.Log("Tag Count : " .. self.owner:GetTagCount())
    self.owner:ClearAllTags()
    Debug.Log("Tag Count : " .. self.owner:GetTagCount())

    Debug.Log("Working")
end

return player