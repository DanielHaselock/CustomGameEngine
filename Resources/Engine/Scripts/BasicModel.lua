local player =
{
    elapsed = 0,
    Timer = 0,
--tests
}
function player:OnBegin()
    Debug.Log("Start")

end


function player:OnUpdate(DeltaTime)
    self.elapsed = self.elapsed + DeltaTime
  --  self.owner:GetTransform(0):SetLocalPosition(Vector3.new(math.sin(self.elapsed), math.sin(self.elapsed),math.sin(self.elapsed)))
   -- self.owner:GetTransform(0):SetLocalRotation(Quaternion.new(0, math.sin(self.elapsed)* 3.1415,0, 1))
   Debug.Log("Elapsed time :" .. self.elapsed)
    if self.owner:GetModel(0) then
        if math.floor(self.elapsed) % 3 == 0 then
            self.owner:GetModel(0):SetModel("Resources/Engine/Models/Torus.fbx")
        elseif math.floor(self.elapsed) % 3 == 1 then
            self.owner:GetModel(0):SetModel("Resources/Engine/Models/Sphere.fbx")
        else
            self.owner:GetModel(0):SetModel("Resources/Engine/Models/Pyramid.fbx")
        end  
    end


end

return player